

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
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
jr2_a0_dnx_data_field_base_ipmf1_nof_ffc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_ffc_groups;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_one_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_one_lower;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_one_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_one_upper;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_two_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_two_lower;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_two_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_two_upper;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_three_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_three_lower;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_three_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_three_upper;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 47;

    
    define->data = 47;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_four_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_four_lower;
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
jr2_a0_dnx_data_field_base_ipmf1_ffc_group_four_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_ffc_group_four_upper;
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
jr2_a0_dnx_data_field_base_ipmf1_program_selection_cam_mask_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_program_selection_cam_mask_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 95;

    
    define->data = 95;

    
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
jr2_a0_dnx_data_field_base_ipmf1_nof_80B_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_80B_zones;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_key_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_key_zones;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_key_zone_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_key_zone_bits;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_fes_key_selects_on_one_actions_line_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fes_key_selects_on_one_actions_line;
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
jr2_a0_dnx_data_field_base_ipmf1_fem_condition_ms_bit_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_programs;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_programs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_programs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_programs_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_programs_get(unit);

    
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
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_condition_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_condition;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_condition_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_condition;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_condition_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_condition_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_map_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_index;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_map_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_map_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_map_index_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_map_index_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_id;
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
    
    define->default_data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_id_get(unit);

    
    define->data = 1 << dnx_data_field.base_ipmf1.nof_bits_in_fem_id_get(unit);

    
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
jr2_a0_dnx_data_field_base_ipmf1_nof_fems_per_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fems_per_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit) * dnx_data_field.base_ipmf1.nof_fem_array_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit) * dnx_data_field.base_ipmf1.nof_fem_array_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_base_ipmf1_nof_fem_action_overriding_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_fem_action_overriding_bits;
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
jr2_a0_dnx_data_field_base_ipmf1_default_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_default_strength;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode;
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
jr2_a0_dnx_data_field_base_ipmf1_nof_compare_keys_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_nof_compare_keys_in_compare_mode;
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
jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf1;
    int define_index = dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit;
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
jr2_a0_dnx_data_field_base_ipmf2_nof_ffc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_ffc_groups;
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
jr2_a0_dnx_data_field_base_ipmf2_ffc_group_one_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_ffc_group_one_lower;
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
jr2_a0_dnx_data_field_base_ipmf2_ffc_group_one_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_ffc_group_one_upper;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
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
jr2_a0_dnx_data_field_base_ipmf2_program_selection_cam_mask_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_program_selection_cam_mask_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 95;

    
    define->data = 95;

    
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
jr2_a0_dnx_data_field_base_ipmf2_nof_80B_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_80B_zones;
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
jr2_a0_dnx_data_field_base_ipmf2_nof_key_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_key_zones;
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
jr2_a0_dnx_data_field_base_ipmf2_nof_key_zone_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_nof_key_zone_bits;
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
jr2_a0_dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf2;
    int define_index = dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit;
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
jr2_a0_dnx_data_field_base_ipmf3_nof_ffc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_ffc_groups;
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
jr2_a0_dnx_data_field_base_ipmf3_ffc_group_one_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_ffc_group_one_lower;
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
jr2_a0_dnx_data_field_base_ipmf3_ffc_group_one_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_ffc_group_one_upper;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 31;

    
    define->data = 31;

    
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
jr2_a0_dnx_data_field_base_ipmf3_program_selection_cam_mask_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_program_selection_cam_mask_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 95;

    
    define->data = 95;

    
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
jr2_a0_dnx_data_field_base_ipmf3_nof_80B_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_80B_zones;
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
jr2_a0_dnx_data_field_base_ipmf3_nof_key_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_key_zones;
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
jr2_a0_dnx_data_field_base_ipmf3_nof_key_zone_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_key_zone_bits;
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
jr2_a0_dnx_data_field_base_ipmf3_nof_fes_key_selects_on_one_actions_line_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_nof_fes_key_selects_on_one_actions_line;
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
jr2_a0_dnx_data_field_base_ipmf3_default_strength_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_default_strength;
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
jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ipmf3;
    int define_index = dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit;
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
        "In J1 mode 1 FES is used to send UDHBase action\n"
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
jr2_a0_dnx_data_field_base_epmf_nof_ffc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_ffc_groups;
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
jr2_a0_dnx_data_field_base_epmf_ffc_group_one_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_ffc_group_one_lower;
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
jr2_a0_dnx_data_field_base_epmf_ffc_group_one_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_ffc_group_one_upper;
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
jr2_a0_dnx_data_field_base_epmf_program_selection_cam_mask_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_program_selection_cam_mask_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37;

    
    define->data = 37;

    
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
jr2_a0_dnx_data_field_base_epmf_nof_80B_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_80B_zones;
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
jr2_a0_dnx_data_field_base_epmf_nof_key_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_key_zones;
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
jr2_a0_dnx_data_field_base_epmf_nof_key_zone_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_key_zone_bits;
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
jr2_a0_dnx_data_field_base_epmf_nof_fes_key_selects_on_one_actions_line_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_nof_fes_key_selects_on_one_actions_line;
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
jr2_a0_dnx_data_field_base_epmf_fes_key_select_for_zero_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_epmf;
    int define_index = dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit;
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
jr2_a0_dnx_data_field_base_ifwd2_nof_ffc_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_nof_ffc_groups;
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
jr2_a0_dnx_data_field_base_ifwd2_ffc_group_one_lower_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_ffc_group_one_lower;
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
jr2_a0_dnx_data_field_base_ifwd2_ffc_group_one_upper_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_base_ifwd2;
    int define_index = dnx_data_field_base_ifwd2_define_ffc_group_one_upper;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 37;

    
    define->data = 37;

    
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
    table->values[51].default_val = "0";
    table->values[52].default_val = "0";
    table->values[53].default_val = "0";
    table->values[54].default_val = "0";
    table->values[55].default_val = "0";
    table->values[56].default_val = "0";
    table->values[57].default_val = "0";
    table->values[58].default_val = "0";
    table->values[59].default_val = "0";
    table->values[60].default_val = "0";
    table->values[61].default_val = "0";
    table->values[62].default_val = "0";
    table->values[63].default_val = "0";
    table->values[64].default_val = "0";
    table->values[65].default_val = "0";
    table->values[66].default_val = "0";
    table->values[67].default_val = "0";
    table->values[68].default_val = "0";
    table->values[69].default_val = "0";
    table->values[70].default_val = "0";
    table->values[71].default_val = "0";
    table->values[72].default_val = "0";
    table->values[73].default_val = "-1";
    table->values[74].default_val = "FALSE";
    table->values[75].default_val = "FALSE";
    table->values[76].default_val = "0";
    table->values[77].default_val = "0";
    table->values[78].default_val = "0";
    table->values[79].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_stage_stage_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_stage_table_stage_info");

    
    default_data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = FALSE;
    default_data->nof_ffc = 0;
    default_data->nof_ffc_groups = 0;
    default_data->ffc_group_one_lower = 0;
    default_data->ffc_group_one_upper = 0;
    default_data->ffc_group_two_lower = 0;
    default_data->ffc_group_two_upper = 0;
    default_data->ffc_group_three_lower = 0;
    default_data->ffc_group_three_upper = 0;
    default_data->ffc_group_four_lower = 0;
    default_data->ffc_group_four_upper = 0;
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
    default_data->program_selection_cam_mask_nof_bits = 0;
    default_data->cs_scratch_pad_size = 0;
    default_data->cs_container_5_selected_bits_size = 0;
    default_data->nof_contexts = 0;
    default_data->nof_link_profiles = 0;
    default_data->nof_cs_lines = 0;
    default_data->nof_actions = 0;
    default_data->nof_qualifiers = 0;
    default_data->nof_80B_zones = 0;
    default_data->nof_key_zones = 0;
    default_data->nof_key_zone_bits = 0;
    default_data->nof_bits_in_fes_action = 0;
    default_data->nof_bits_in_fes_key_select = 0;
    default_data->nof_fes_key_selects_on_one_actions_line = 0;
    default_data->fem_condition_ms_bit_min_value = 0;
    default_data->fem_condition_ms_bit_max_value = 0;
    default_data->nof_bits_in_fem_programs = 0;
    default_data->nof_fem_programs = 0;
    default_data->log_nof_bits_in_fem_key_select = 0;
    default_data->nof_bits_in_fem_key_select = 0;
    default_data->fem_key_select_resolution_in_bits = 0;
    default_data->log_nof_bits_in_fem_map_data_field = 0;
    default_data->nof_bits_in_fem_map_data_field = 0;
    default_data->nof_bits_in_fem_action_fems_2_15 = 0;
    default_data->nof_bits_in_fem_action_fems_0_1 = 0;
    default_data->nof_bits_in_fem_action = 0;
    default_data->nof_bits_in_fem_condition = 0;
    default_data->nof_fem_condition = 0;
    default_data->nof_bits_in_fem_map_index = 0;
    default_data->nof_fem_map_index = 0;
    default_data->nof_bits_in_fem_id = 0;
    default_data->nof_fem_id = 0;
    default_data->num_fems_with_short_action = 0;
    default_data->num_bits_in_fem_field_select = 0;
    default_data->nof_fem_id_per_array = 0;
    default_data->nof_fem_array = 0;
    default_data->nof_fems_per_context = 0;
    default_data->nof_fem_action_overriding_bits = 0;
    default_data->default_strength = 0;
    default_data->pbus_header_length = 0;
    default_data->nof_layer_records = 0;
    default_data->layer_record_size = 0;
    default_data->nof_compare_pairs_in_compare_mode = 0;
    default_data->nof_compare_keys_in_compare_mode = 0;
    default_data->compare_key_size = 0;
    default_data->nof_l4_ops_ranges_legacy = 0;
    default_data->nof_ext_l4_ops_ranges = 0;
    default_data->nof_pkt_hdr_ranges = 0;
    default_data->nof_out_lif_ranges = 0;
    default_data->fes_key_select_for_zero_bit = 0;
    default_data->fes_shift_for_zero_bit = -1;
    default_data->uses_small_exem = FALSE;
    default_data->uses_large_exem = FALSE;
    default_data->cmp_selection = 0;
    default_data->nof_bits_main_pbus = 0;
    default_data->nof_bits_native_pbus = 0;
    default_data->dir_ext_single_key_size = 0;
    
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
        data->nof_ffc_groups = dnx_data_field.base_ipmf1.nof_ffc_groups_get(unit);
        data->ffc_group_one_lower = dnx_data_field.base_ipmf1.ffc_group_one_lower_get(unit);
        data->ffc_group_one_upper = dnx_data_field.base_ipmf1.ffc_group_one_upper_get(unit);
        data->ffc_group_two_lower = dnx_data_field.base_ipmf1.ffc_group_two_lower_get(unit);
        data->ffc_group_two_upper = dnx_data_field.base_ipmf1.ffc_group_two_upper_get(unit);
        data->ffc_group_three_lower = dnx_data_field.base_ipmf1.ffc_group_three_lower_get(unit);
        data->ffc_group_three_upper = dnx_data_field.base_ipmf1.ffc_group_three_upper_get(unit);
        data->ffc_group_four_lower = dnx_data_field.base_ipmf1.ffc_group_four_lower_get(unit);
        data->ffc_group_four_upper = dnx_data_field.base_ipmf1.ffc_group_four_upper_get(unit);
        data->nof_keys = dnx_data_field.base_ipmf1.nof_keys_get(unit);
        data->nof_keys_alloc = dnx_data_field.base_ipmf1.nof_keys_alloc_get(unit);
        data->nof_keys_alloc_for_tcam = dnx_data_field.base_ipmf1.nof_keys_alloc_for_tcam_get(unit);
        data->nof_keys_alloc_for_exem = dnx_data_field.base_ipmf1.nof_keys_alloc_for_exem_get(unit);
        data->nof_keys_alloc_for_mdb_dt = dnx_data_field.base_ipmf1.nof_keys_alloc_for_mdb_dt_get(unit);
        data->nof_masks_per_fes = dnx_data_field.base_ipmf1.nof_masks_per_fes_get(unit);
        data->nof_fes_id_per_array = dnx_data_field.base_ipmf1.nof_fes_id_per_array_get(unit);
        data->nof_fes_array = dnx_data_field.base_ipmf1.nof_fes_array_get(unit);
        data->nof_fes_instruction_per_context = dnx_data_field.base_ipmf1.nof_fes_instruction_per_context_get(unit);
        data->nof_fes_programs = dnx_data_field.base_ipmf1.nof_fes_programs_get(unit);
        data->nof_prog_per_fes = dnx_data_field.base_ipmf1.nof_prog_per_fes_get(unit);
        data->program_selection_cam_mask_nof_bits = dnx_data_field.base_ipmf1.program_selection_cam_mask_nof_bits_get(unit);
        data->cs_container_5_selected_bits_size = dnx_data_field.base_ipmf1.cs_container_5_selected_bits_size_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf1.nof_contexts_get(unit);
        data->nof_link_profiles = dnx_data_field.base_ipmf1.nof_link_profiles_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf1.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_ipmf1.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf1.nof_qualifiers_get(unit);
        data->nof_80B_zones = dnx_data_field.base_ipmf1.nof_80B_zones_get(unit);
        data->nof_key_zones = dnx_data_field.base_ipmf1.nof_key_zones_get(unit);
        data->nof_key_zone_bits = dnx_data_field.base_ipmf1.nof_key_zone_bits_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit);
        data->nof_fes_key_selects_on_one_actions_line = dnx_data_field.base_ipmf1.nof_fes_key_selects_on_one_actions_line_get(unit);
        data->fem_condition_ms_bit_min_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_min_value_get(unit);
        data->fem_condition_ms_bit_max_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_max_value_get(unit);
        data->nof_bits_in_fem_programs = dnx_data_field.base_ipmf1.nof_bits_in_fem_programs_get(unit);
        data->nof_fem_programs = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);
        data->log_nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);
        data->nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fem_key_select_get(unit);
        data->fem_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fem_key_select_resolution_in_bits_get(unit);
        data->log_nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_action_fems_2_15 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit);
        data->nof_bits_in_fem_action_fems_0_1 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit);
        data->nof_bits_in_fem_action = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);
        data->nof_bits_in_fem_condition = dnx_data_field.base_ipmf1.nof_bits_in_fem_condition_get(unit);
        data->nof_fem_condition = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);
        data->nof_bits_in_fem_map_index = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_index_get(unit);
        data->nof_fem_map_index = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);
        data->nof_bits_in_fem_id = dnx_data_field.base_ipmf1.nof_bits_in_fem_id_get(unit);
        data->nof_fem_id = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);
        data->num_fems_with_short_action = dnx_data_field.base_ipmf1.num_fems_with_short_action_get(unit);
        data->num_bits_in_fem_field_select = dnx_data_field.base_ipmf1.num_bits_in_fem_field_select_get(unit);
        data->nof_fem_id_per_array = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit);
        data->nof_fem_array = dnx_data_field.base_ipmf1.nof_fem_array_get(unit);
        data->nof_fems_per_context = dnx_data_field.base_ipmf1.nof_fems_per_context_get(unit);
        data->nof_fem_action_overriding_bits = dnx_data_field.base_ipmf1.nof_fem_action_overriding_bits_get(unit);
        data->default_strength = dnx_data_field.base_ipmf1.default_strength_get(unit);
        data->pbus_header_length = dnx_data_field.qual.ingress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->nof_compare_pairs_in_compare_mode = dnx_data_field.base_ipmf1.nof_compare_pairs_in_compare_mode_get(unit);
        data->nof_compare_keys_in_compare_mode = dnx_data_field.base_ipmf1.nof_compare_keys_in_compare_mode_get(unit);
        data->compare_key_size = dnx_data_field.base_ipmf1.compare_key_size_get(unit);
        data->nof_l4_ops_ranges_legacy = dnx_data_field.base_ipmf1.nof_l4_ops_ranges_legacy_get(unit);
        data->nof_ext_l4_ops_ranges = dnx_data_field.base_ipmf1.nof_ext_l4_ops_ranges_get(unit);
        data->nof_pkt_hdr_ranges = dnx_data_field.base_ipmf1.nof_pkt_hdr_ranges_get(unit);
        data->nof_out_lif_ranges = dnx_data_field.base_ipmf1.nof_out_lif_ranges_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf1.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf1.fes_shift_for_zero_bit_get(unit);
        data->uses_small_exem = FALSE;
        data->uses_large_exem = TRUE;
        data->cmp_selection = dnx_data_field.base_ipmf1.cmp_selection_get(unit);
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf1_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf1.dir_ext_single_key_size_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ipmf2.nof_ffc_get(unit);
        data->nof_ffc_groups = dnx_data_field.base_ipmf2.nof_ffc_groups_get(unit);
        data->ffc_group_one_lower = dnx_data_field.base_ipmf2.ffc_group_one_lower_get(unit);
        data->ffc_group_one_upper = dnx_data_field.base_ipmf2.ffc_group_one_upper_get(unit);
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
        data->program_selection_cam_mask_nof_bits = dnx_data_field.base_ipmf2.program_selection_cam_mask_nof_bits_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf2.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf2.nof_cs_lines_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf2.nof_qualifiers_get(unit);
        data->nof_80B_zones = dnx_data_field.base_ipmf2.nof_80B_zones_get(unit);
        data->nof_key_zones = dnx_data_field.base_ipmf2.nof_key_zones_get(unit);
        data->nof_key_zone_bits = dnx_data_field.base_ipmf2.nof_key_zone_bits_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf1.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fes_key_select_get(unit);
        data->fem_condition_ms_bit_min_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_min_value_get(unit);
        data->fem_condition_ms_bit_max_value = dnx_data_field.base_ipmf1.fem_condition_ms_bit_max_value_get(unit);
        data->nof_bits_in_fem_programs = dnx_data_field.base_ipmf1.nof_bits_in_fem_programs_get(unit);
        data->nof_fem_programs = dnx_data_field.base_ipmf1.nof_fem_programs_get(unit);
        data->log_nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_key_select_get(unit);
        data->nof_bits_in_fem_key_select = dnx_data_field.base_ipmf1.nof_bits_in_fem_key_select_get(unit);
        data->fem_key_select_resolution_in_bits = dnx_data_field.base_ipmf1.fem_key_select_resolution_in_bits_get(unit);
        data->log_nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.log_nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_map_data_field = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_data_field_get(unit);
        data->nof_bits_in_fem_action_fems_2_15 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_2_15_get(unit);
        data->nof_bits_in_fem_action_fems_0_1 = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_fems_0_1_get(unit);
        data->nof_bits_in_fem_action = dnx_data_field.base_ipmf1.nof_bits_in_fem_action_get(unit);
        data->nof_bits_in_fem_condition = dnx_data_field.base_ipmf1.nof_bits_in_fem_condition_get(unit);
        data->nof_fem_condition = dnx_data_field.base_ipmf1.nof_fem_condition_get(unit);
        data->nof_bits_in_fem_map_index = dnx_data_field.base_ipmf1.nof_bits_in_fem_map_index_get(unit);
        data->nof_fem_map_index = dnx_data_field.base_ipmf1.nof_fem_map_index_get(unit);
        data->nof_bits_in_fem_id = dnx_data_field.base_ipmf1.nof_bits_in_fem_id_get(unit);
        data->nof_fem_id = dnx_data_field.base_ipmf1.nof_fem_id_get(unit);
        data->num_fems_with_short_action = dnx_data_field.base_ipmf1.num_fems_with_short_action_get(unit);
        data->num_bits_in_fem_field_select = dnx_data_field.base_ipmf1.num_bits_in_fem_field_select_get(unit);
        data->nof_fem_id_per_array = dnx_data_field.base_ipmf1.nof_fem_id_per_array_get(unit);
        data->nof_fem_array = dnx_data_field.base_ipmf1.nof_fem_array_get(unit);
        data->nof_fems_per_context = dnx_data_field.base_ipmf1.nof_fems_per_context_get(unit);
        data->nof_fem_action_overriding_bits = dnx_data_field.base_ipmf1.nof_fem_action_overriding_bits_get(unit);
        data->pbus_header_length = dnx_data_field.qual.ingress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf2.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf2.fes_shift_for_zero_bit_get(unit);
        data->uses_small_exem = TRUE;
        data->uses_large_exem = FALSE;
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf1_pbus_size_get(unit);
        data->nof_bits_native_pbus = dnx_data_field.qual.ipmf2_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf2.dir_ext_single_key_size_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ipmf3.nof_ffc_get(unit);
        data->nof_ffc_groups = dnx_data_field.base_ipmf3.nof_ffc_groups_get(unit);
        data->ffc_group_one_lower = dnx_data_field.base_ipmf3.ffc_group_one_lower_get(unit);
        data->ffc_group_one_upper = dnx_data_field.base_ipmf3.ffc_group_one_upper_get(unit);
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
        data->program_selection_cam_mask_nof_bits = dnx_data_field.base_ipmf3.program_selection_cam_mask_nof_bits_get(unit);
        data->cs_scratch_pad_size = dnx_data_field.base_ipmf3.cs_scratch_pad_size_get(unit);
        data->nof_contexts = dnx_data_field.base_ipmf3.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_ipmf3.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_ipmf3.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_ipmf3.nof_qualifiers_get(unit);
        data->nof_80B_zones = dnx_data_field.base_ipmf3.nof_80B_zones_get(unit);
        data->nof_key_zones = dnx_data_field.base_ipmf3.nof_key_zones_get(unit);
        data->nof_key_zone_bits = dnx_data_field.base_ipmf3.nof_key_zone_bits_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_ipmf3.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_ipmf3.nof_bits_in_fes_key_select_get(unit);
        data->nof_fes_key_selects_on_one_actions_line = dnx_data_field.base_ipmf3.nof_fes_key_selects_on_one_actions_line_get(unit);
        data->default_strength = dnx_data_field.base_ipmf3.default_strength_get(unit);
        data->nof_layer_records = dnx_data_field.qual.ingress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.ingress_layer_record_size_get(unit);
        data->nof_out_lif_ranges = dnx_data_field.base_ipmf3.nof_out_lif_ranges_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_ipmf3.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_ipmf3.fes_shift_for_zero_bit_get(unit);
        data->uses_small_exem = TRUE;
        data->uses_large_exem = FALSE;
        data->nof_bits_main_pbus = dnx_data_field.qual.ipmf3_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_ipmf3.dir_ext_single_key_size_get(unit);
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_epmf.nof_ffc_get(unit);
        data->nof_ffc_groups = dnx_data_field.base_epmf.nof_ffc_groups_get(unit);
        data->ffc_group_one_lower = dnx_data_field.base_epmf.ffc_group_one_lower_get(unit);
        data->ffc_group_one_upper = dnx_data_field.base_epmf.ffc_group_one_upper_get(unit);
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
        data->program_selection_cam_mask_nof_bits = dnx_data_field.base_epmf.program_selection_cam_mask_nof_bits_get(unit);
        data->nof_contexts = dnx_data_field.base_epmf.nof_contexts_get(unit);
        data->nof_cs_lines = dnx_data_field.base_epmf.nof_cs_lines_get(unit);
        data->nof_actions = dnx_data_field.base_epmf.nof_actions_get(unit);
        data->nof_qualifiers = dnx_data_field.base_epmf.nof_qualifiers_get(unit);
        data->nof_80B_zones = dnx_data_field.base_epmf.nof_80B_zones_get(unit);
        data->nof_key_zones = dnx_data_field.base_epmf.nof_key_zones_get(unit);
        data->nof_key_zone_bits = dnx_data_field.base_epmf.nof_key_zone_bits_get(unit);
        data->nof_bits_in_fes_action = dnx_data_field.base_epmf.nof_bits_in_fes_action_get(unit);
        data->nof_bits_in_fes_key_select = dnx_data_field.base_epmf.nof_bits_in_fes_key_select_get(unit);
        data->nof_fes_key_selects_on_one_actions_line = dnx_data_field.base_epmf.nof_fes_key_selects_on_one_actions_line_get(unit);
        data->pbus_header_length = dnx_data_field.qual.egress_pbus_header_length_get(unit);
        data->nof_layer_records = dnx_data_field.qual.egress_nof_layer_records_get(unit);
        data->layer_record_size = dnx_data_field.qual.egress_layer_record_size_get(unit);
        data->nof_l4_ops_ranges_legacy = dnx_data_field.base_epmf.nof_l4_ops_ranges_legacy_get(unit);
        data->fes_key_select_for_zero_bit = dnx_data_field.base_epmf.fes_key_select_for_zero_bit_get(unit);
        data->fes_shift_for_zero_bit = dnx_data_field.base_epmf.fes_shift_for_zero_bit_get(unit);
        data->uses_small_exem = FALSE;
        data->uses_large_exem = TRUE;
        data->nof_bits_main_pbus = dnx_data_field.qual.epmf_pbus_size_get(unit);
        data->dir_ext_single_key_size = dnx_data_field.base_epmf.dir_ext_single_key_size_get(unit);
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size)
    {
        data = (dnx_data_field_stage_stage_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, 0);
        data->valid = TRUE;
        data->nof_ffc = dnx_data_field.base_ifwd2.nof_ffc_get(unit);
        data->nof_ffc_groups = dnx_data_field.base_ifwd2.nof_ffc_groups_get(unit);
        data->ffc_group_one_lower = dnx_data_field.base_ifwd2.ffc_group_one_lower_get(unit);
        data->ffc_group_one_upper = dnx_data_field.base_ifwd2.ffc_group_one_upper_get(unit);
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_field_kbp_nof_fgs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_nof_fgs;
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
jr2_a0_dnx_data_field_kbp_max_single_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_max_single_key_size;
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
jr2_a0_dnx_data_field_kbp_nof_acl_keys_master_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_nof_acl_keys_master_max;
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
jr2_a0_dnx_data_field_kbp_nof_acl_keys_fg_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_nof_acl_keys_fg_max;
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
jr2_a0_dnx_data_field_kbp_min_acl_nof_ffc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_min_acl_nof_ffc;
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
jr2_a0_dnx_data_field_kbp_max_fwd_context_num_for_one_apptype_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_max_fwd_context_num_for_one_apptype;
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
jr2_a0_dnx_data_field_kbp_max_acl_context_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_max_acl_context_num;
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
jr2_a0_dnx_data_field_kbp_size_apptype_profile_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_size_apptype_profile_id;
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
jr2_a0_dnx_data_field_kbp_key_bmp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_key_bmp;
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
jr2_a0_dnx_data_field_kbp_apptype_user_1st_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_apptype_user_1st;
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
jr2_a0_dnx_data_field_kbp_apptype_user_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_apptype_user_nof;
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
jr2_a0_dnx_data_field_kbp_max_payload_size_per_opcode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int define_index = dnx_data_field_kbp_define_max_payload_size_per_opcode;
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
jr2_a0_dnx_data_field_kbp_ffc_to_quad_and_group_map_set(
    int unit)
{
    int ffc_id_index;
    dnx_data_field_kbp_ffc_to_quad_and_group_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_kbp;
    int table_index = dnx_data_field_kbp_table_ffc_to_quad_and_group_map;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_kbp_ffc_to_quad_and_group_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_kbp_table_ffc_to_quad_and_group_map");

    
    default_data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quad_id = SAL_UINT32_MAX;
    default_data->group_id = SAL_UINT32_MAX;
    
    for (ffc_id_index = 0; ffc_id_index < table->keys[0].size; ffc_id_index++)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ffc_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->quad_id = 0;
        data->group_id = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->quad_id = 0;
        data->group_id = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->quad_id = 1;
        data->group_id = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->quad_id = 2;
        data->group_id = 2;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->quad_id = 3;
        data->group_id = 2;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->quad_id = 4;
        data->group_id = 3;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->quad_id = 5;
        data->group_id = 3;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->quad_id = 6;
        data->group_id = 3;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->quad_id = 7;
        data->group_id = 3;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->quad_id = 8;
        data->group_id = 4;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_field_kbp_ffc_to_quad_and_group_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->quad_id = 9;
        data->group_id = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_field_tcam_tcam_320b_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int feature_index = dnx_data_field_tcam_tcam_320b_support;
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
jr2_a0_dnx_data_field_tcam_entry_size_half_payload_hw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_entry_size_half_payload_hw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.tcam.action_size_half_get(unit);

    
    define->data = dnx_data_field.tcam.action_size_half_get(unit);

    
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
jr2_a0_dnx_data_field_tcam_small_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_small_bank_size;
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
jr2_a0_dnx_data_field_tcam_cascaded_data_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_cascaded_data_nof_bits;
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
jr2_a0_dnx_data_field_tcam_big_bank_key_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_big_bank_key_nof_bits;
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
    
    define->default_data = 64;

    
    define->data = 64;

    
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
jr2_a0_dnx_data_field_tcam_nof_keys_in_double_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_keys_in_double_key;
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
jr2_a0_dnx_data_field_tcam_nof_keys_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_tcam;
    int define_index = dnx_data_field_tcam_define_nof_keys_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.tcam.nof_keys_in_double_key_get(unit);

    
    define->data = dnx_data_field.tcam.nof_keys_in_double_key_get(unit);

    
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
    
    define->default_data = UTILEX_MAX3(dnx_data_field.tcam.nof_keys_max_get(unit), dnx_data_field.kbp.nof_acl_keys_master_max_get(unit), dnx_data_field.common_max_val.nof_compare_pairs_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.tcam.nof_keys_max_get(unit), dnx_data_field.kbp.nof_acl_keys_master_max_get(unit), dnx_data_field.common_max_val.nof_compare_pairs_get(unit));

    
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
    
    define->default_data = UTILEX_MAX4(dnx_data_field.tcam.action_size_double_get(unit), dnx_data_field.tcam.key_size_double_get(unit), dnx_data_field.exem.large_max_result_size_get(unit), dnx_data_field.kbp.max_payload_size_per_opcode_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.tcam.action_size_double_get(unit), dnx_data_field.tcam.key_size_double_get(unit), dnx_data_field.exem.large_max_result_size_get(unit), dnx_data_field.kbp.max_payload_size_per_opcode_get(unit));

    
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
    table->keys[1].size = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    table->info_get.key_size[1] = DBAL_NOF_ENUM_FIELD_IO_VALUES;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
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
    default_data->key_select[0] = -1;
    default_data->num_bits[0] = 0;
    default_data->lsb[0] = 0;
    default_data->num_bits_not_on_key[0] = 0;
    
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

    
    table->keys[0].size = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_FIELD_IO_VALUES;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_fem_key_select_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_fem_table_key_select_properties");

    
    default_data = (dnx_data_field_fem_key_select_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->key_select[0] = -1;
    default_data->num_bits[0] = 0;
    default_data->lsb[0] = 0;
    default_data->num_bits_not_on_key[0] = 0;
    
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
        data->num_bits_not_on_key[1] = 40;
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
        data->num_bits_not_on_key[1] = 40;
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
jr2_a0_dnx_data_field_context_default_stacking_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_context;
    int define_index = dnx_data_field_context_define_default_stacking_context;
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
jr2_a0_dnx_data_field_preselector_default_oam_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_oam_presel_id_ipmf1;
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
jr2_a0_dnx_data_field_preselector_default_stacking_presel_id_ipmf1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_stacking_presel_id_ipmf1;
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
jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_preselector;
    int define_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3;
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
    
    define->default_data = 3225;

    
    define->data = 3225;

    
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
jr2_a0_dnx_data_field_qual_ac_lif_wide_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int define_index = dnx_data_field_qual_define_ac_lif_wide_size;
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
jr2_a0_dnx_data_field_qual_params_set(
    int unit)
{
    int stage_index;
    int qual_index;
    dnx_data_field_qual_params_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int table_index = dnx_data_field_qual_table_params;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_STAGE_NOF;
    table->info_get.key_size[0] = DNX_FIELD_STAGE_NOF;
    table->keys[1].size = 256;
    table->info_get.key_size[1] = 256;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "unsupported";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_qual_params_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_field_qual_table_params");

    
    default_data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = -1;
    default_data->signal = "unsupported";
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (qual_index = 0; qual_index < table->keys[1].size; qual_index++)
        {
            data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, qual_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1734;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_LAYER_INDEX);
        data->offset = 1408;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_RPF_ROUTE_VALID);
        data->offset = 1411;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE);
        data->offset = 1412;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT);
        data->offset = 1419;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE);
        data->offset = 1515;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT);
        data->offset = 1522;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MP_PROFILE_SEL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MP_PROFILE_SEL);
        data->offset = 1618;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 1619;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ACL_CONTEXT);
        data->offset = 1620;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_CONTEXT);
        data->offset = 1626;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_NASID);
        data->offset = 1632;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ENABLE_PP_INJECT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ENABLE_PP_INJECT);
        data->offset = 1637;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_EEI);
        data->offset = 1638;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 1662;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1663;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 1672;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 1691;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1712;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_DOMAIN);
        data->offset = 1716;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VRF_VALUE);
        data->offset = 1716;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 1734;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 1744;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_0);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_1);
        data->offset = 1876;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_2);
        data->offset = 1904;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GENERAL_DATA_3);
        data->offset = 2032;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_AC_IN_LIF_WIDE_DATA);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MACT_ENTRY_GROUPING < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MACT_ENTRY_GROUPING);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IEEE1588_COMMAND);
        data->offset = 2258;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2260;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2261;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2262;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2270;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2278;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_0);
        data->offset = 2286;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_1);
        data->offset = 2308;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PP_PORT);
        data->offset = 2330;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 2338;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2343;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_LEARN_INFO);
        data->offset = 2344;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MEM_SOFT_ERR);
        data->offset = 2504;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_NWK_QOS);
        data->offset = 2505;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_OAM_OFFSET);
        data->offset = 2513;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2521;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_OAM_OPCODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_OAM_OPCODE);
        data->offset = 2529;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_OAM_YOUR_DISCR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_OAM_YOUR_DISCR);
        data->offset = 2533;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MY_CFM_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MY_CFM_MAC);
        data->offset = 2534;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_IS_OAM < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_IS_OAM);
        data->offset = 2535;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_IS_BFD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_IS_BFD);
        data->offset = 2536;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MDL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MDL);
        data->offset = 2537;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_0);
        data->offset = 2540;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_0_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_0_VALID);
        data->offset = 2562;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_1);
        data->offset = 2563;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_1_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_1_VALID);
        data->offset = 2585;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_2);
        data->offset = 2586;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_2_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_LIF_2_VALID);
        data->offset = 2608;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_0);
        data->offset = 2609;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_1);
        data->offset = 2612;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VTT_OAM_PCP_2);
        data->offset = 2615;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2618;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2640;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PRT_QUALIFIER);
        data->offset = 2662;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2665;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_IS_APPLET);
        data->offset = 2673;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2674;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2675;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_DP);
        data->offset = 2676;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ECN);
        data->offset = 2678;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_TC);
        data->offset = 2680;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_PTC);
        data->offset = 2683;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_RPF_DST);
        data->offset = 2691;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_RPF_DST_VALID);
        data->offset = 2712;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_SNOOP_CODE);
        data->offset = 2713;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_SNOOP_STRENGTH);
        data->offset = 2722;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_MIRROR_CODE);
        data->offset = 2725;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 2734;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_SRC_SYS_PORT);
        data->offset = 2743;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_0);
        data->offset = 2759;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_1);
        data->offset = 2779;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_2);
        data->offset = 2799;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_3);
        data->offset = 2819;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_4);
        data->offset = 2839;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ID_5);
        data->offset = 2859;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_0);
        data->offset = 2879;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_1);
        data->offset = 2884;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_2);
        data->offset = 2889;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_3);
        data->offset = 2894;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_4);
        data->offset = 2899;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FLP_STATISTICS_ATR_5);
        data->offset = 2904;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VLAN_EDIT_CMD);
        data->offset = 2909;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_TRACE_PACKET);
        data->offset = 2948;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IN_TTL);
        data->offset = 2949;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_LIF_OAM_TRAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_LIF_OAM_TRAP_PROFILE);
        data->offset = 2957;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VISIBILITY_CLEAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VISIBILITY_CLEAR);
        data->offset = 2959;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VISIBILITY);
        data->offset = 2960;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_RPF_OUT_LIF);
        data->offset = 2961;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_SA_DROP_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_SA_DROP_INDEX);
        data->offset = 2983;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX);
        data->offset = 2985;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX);
        data->offset = 2987;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 2988;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_VERSION_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_VERSION_ERROR);
        data->offset = 2994;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_CHECKSUM_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_CHECKSUM_ERROR);
        data->offset = 2995;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_TOTAL_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_TOTAL_LENGTH_ERROR);
        data->offset = 2996;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_OPTIONS_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_OPTIONS_ERROR);
        data->offset = 2997;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_SIP_EQUAL_DIP_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_SIP_EQUAL_DIP_ERROR);
        data->offset = 2998;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_DIP_ZERO_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_DIP_ZERO_ERROR);
        data->offset = 2999;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_SIP_IS_MC_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_SIP_IS_MC_ERROR);
        data->offset = 3000;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IPV4_HEADER_LENGTH_ERROR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IPV4_HEADER_LENGTH_ERROR);
        data->offset = 3001;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IN_LIF_UNKNOWN_DA_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IN_LIF_UNKNOWN_DA_PROFILE);
        data->offset = 3002;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_NOF_VALID_LM_LIFS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_NOF_VALID_LM_LIFS);
        data->offset = 3004;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_IS_IN_LIF_VALID_FOR_LM < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_IS_IN_LIF_VALID_FOR_LM);
        data->offset = 3006;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH);
        data->offset = 3007;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH);
        data->offset = 3012;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND);
        data->offset = 3017;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND);
        data->offset = 3018;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_STATISTICS_META_DATA);
        data->offset = 3019;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD);
        data->offset = 3035;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_FWD_STAT_ATTRIBUTE_OBJ < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_FWD_STAT_ATTRIBUTE_OBJ);
        data->offset = 3040;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VSI_UNKNOWN_DA_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VSI_UNKNOWN_DA_DESTINATION);
        data->offset = 3060;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_0);
        data->offset = 3081;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_1);
        data->offset = 3097;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_ECMP_LB_KEY_2);
        data->offset = 3113;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_LAG_LB_KEY);
        data->offset = 3129;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_NWK_LB_KEY);
        data->offset = 3145;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_CONTEXT_CONSTANTS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_CONTEXT_CONSTANTS);
        data->offset = 3161;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VW_VIP_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VW_VIP_VALID);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VW_VIP_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VW_VIP_ID);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VW_MEMBER_REFERENCE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VW_MEMBER_REFERENCE);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_EXTERNAL < table->keys[0].size && DNX_FIELD_IFWD2_QUAL_VW_PCC_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VW_PCC_HIT);
        data->offset = 1748;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 2884;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 1866;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PP_PORT_PMF_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PP_PORT_PMF_GENERAL_DATA);
        data->offset = 1907+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 1907;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ALL_ONES);
        data->offset = 3339;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_MEM_SOFT_ERR);
        data->offset = 3338;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ACL_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ACL_CONTEXT);
        data->offset = 3332;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_NASID);
        data->offset = 3327;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT);
        data->offset = 3321;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_RPF_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_RPF_OUT_LIF);
        data->offset = 3299;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_RPF_DEFAULT_ROUTE_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_RPF_DEFAULT_ROUTE_FOUND);
        data->offset = 3298;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_RPF_ROUTE_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_RPF_ROUTE_VALID);
        data->offset = 3297;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_IS_APPLET);
        data->offset = 3296;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_2);
        data->offset = 3168;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_1);
        data->offset = 3040;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_0);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN);
        data->offset = 2894;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VRF_VALUE);
        data->offset = 2894;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 2884;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 2881;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 2880;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 2880;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_VID_2);
        data->offset = 2868;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 2865;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 2864;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 2864;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_VID_1);
        data->offset = 2852;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 2845;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_CMD);
        data->offset = 2845;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_SRC_SYS_PORT);
        data->offset = 2829;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_SNOOP_CODE);
        data->offset = 2820;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_SNOOP_STRENGTH);
        data->offset = 2817;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 2813;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_RPF_DST);
        data->offset = 2792;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_RPF_DST_VALID);
        data->offset = 2791;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_IS_IEEE1588);
        data->offset = 2790;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 2789;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IEEE1588_COMMAND);
        data->offset = 2787;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 2786;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 2778;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_IS_COMPATIBLE_MC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_IS_COMPATIBLE_MC);
        data->offset = 2777;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PRT_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PRT_QUALIFIER);
        data->offset = 2774;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_HEADER_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_HEADER_SIZE);
        data->offset = 2766;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_ID);
        data->offset = 2747;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_UP_MEP);
        data->offset = 2746;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_SUB_TYPE);
        data->offset = 2742;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_OFFSET);
        data->offset = 2734;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_STAMP_OFFSET);
        data->offset = 2726;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_MIRROR_CODE);
        data->offset = 2717;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VISIBILITY);
        data->offset = 2716;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_EXPECTED_WON);
        data->offset = 2711;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_FOUND);
        data->offset = 2710;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_MATCH);
        data->offset = 2709;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_STATION_MOVE);
        data->offset = 2708;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_VLAN);
        data->offset = 2691;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_VSI);
        data->offset = 2673;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_SRC_MAC);
        data->offset = 2625;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_DATA);
        data->offset = 2559;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LEARN_INFO);
        data->offset = 2556;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 2555;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 2554;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GLOB_IN_LIF_1);
        data->offset = 2532;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GLOB_IN_LIF_0);
        data->offset = 2510;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_LIF_PROFILE_1);
        data->offset = 2502;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2494;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 2490;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 2469;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 2460;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 2441;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EEI);
        data->offset = 2417;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_0);
        data->offset = 2416;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_1);
        data->offset = 2415;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_2);
        data->offset = 2414;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_3);
        data->offset = 2413;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_4);
        data->offset = 2412;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_5 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_5);
        data->offset = 2411;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_6 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_6);
        data->offset = 2410;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_7 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_7);
        data->offset = 2409;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_1);
        data->offset = 2321;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_0);
        data->offset = 2161;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PP_PORT);
        data->offset = 2153;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_CORE_ID);
        data->offset = 2152;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_PORT);
        data->offset = 2152;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PTC);
        data->offset = 2144;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_LAG_LB_KEY);
        data->offset = 2128;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_NWK_LB_KEY);
        data->offset = 2112;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_2);
        data->offset = 2096;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_1);
        data->offset = 2080;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_0);
        data->offset = 2064;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2042;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2020;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_TTL);
        data->offset = 2012;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_BIER_STR_OFFSET);
        data->offset = 2004;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_BIER_STR_SIZE);
        data->offset = 2002;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_IS_BIER);
        data->offset = 2001;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_SAVED_CONTEXT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_SAVED_CONTEXT_PROFILE);
        data->offset = 1995;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_OAM_METER_DISABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_OAM_METER_DISABLE);
        data->offset = 1994;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_INCOMING_TAG_STRUCTURE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_INCOMING_TAG_STRUCTURE);
        data->offset = 1989;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_TC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_TC);
        data->offset = 1986;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_DP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_DP);
        data->offset = 1984;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_ECN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_ECN);
        data->offset = 1982;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_NWK_QOS);
        data->offset = 1974;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_TRACE_PACKET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_TRACE_PACKET);
        data->offset = 1973;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PACKET_HEADER_SIZE_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PACKET_HEADER_SIZE_RANGE);
        data->offset = 1971;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 1907;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_IN_PORT_MAPPED_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_IN_PORT_MAPPED_PP_PORT);
        data->offset = 1898;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_PTC_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_PTC_KEY_GEN_VAR);
        data->offset = 1866;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VID_VALID);
        data->offset = 1865;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_L4OPS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_L4OPS);
        data->offset = 1841;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_OBJECT10 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_OBJECT10);
        data->offset = 1838;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_CPU_TRAP_CODE_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_CPU_TRAP_CODE_PROFILE);
        data->offset = 1837;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_TM_FLOW_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_TM_FLOW_ID);
        data->offset = 1817;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_3);
        data->offset = 1797;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_2);
        data->offset = 1777;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_1);
        data->offset = 1757;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_0);
        data->offset = 1737;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_9 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_9);
        data->offset = 1717;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_8 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_8);
        data->offset = 1697;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_7 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_7);
        data->offset = 1677;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_6 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_6);
        data->offset = 1657;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_5 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_5);
        data->offset = 1637;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_4);
        data->offset = 1617;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_3);
        data->offset = 1597;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_2);
        data->offset = 1577;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_1);
        data->offset = 1557;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_0);
        data->offset = 1537;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_TM_FLOW_ATR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_TM_FLOW_ATR);
        data->offset = 1532;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_3);
        data->offset = 1527;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_2);
        data->offset = 1522;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_1);
        data->offset = 1517;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ATR_0);
        data->offset = 1512;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_9 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_9);
        data->offset = 1507;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_8 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_8);
        data->offset = 1502;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_7 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_7);
        data->offset = 1497;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_6 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_6);
        data->offset = 1492;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_5 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_5);
        data->offset = 1487;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_4);
        data->offset = 1482;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_3);
        data->offset = 1477;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_2);
        data->offset = 1472;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_1);
        data->offset = 1467;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_0);
        data->offset = 1462;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_STATISTICS_META_DATA);
        data->offset = 1446;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_CONTEXT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_CONTEXT_KEY_GEN_VAR);
        data->offset = 1414;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_EGRESS_PARSING_INDEX);
        data->offset = 1411;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_FWD_LAYER_INDEX);
        data->offset = 1408;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_AC_IN_LIF_WIDE_DATA);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_MACT_ENTRY_GROUPING < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_MACT_ENTRY_GROUPING);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VW_VIP_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VW_VIP_VALID);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VW_VIP_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VW_VIP_ID);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VW_MEMBER_REFERENCE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VW_MEMBER_REFERENCE);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && DNX_FIELD_IPMF1_QUAL_VW_PCC_HIT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_VW_PCC_HIT);
        data->offset = 2912;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0);
        data->offset = 1320;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1);
        data->offset = 1256;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2);
        data->offset = 1192;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_3);
        data->offset = 1128;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_EXEM_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_EXEM_ACTION);
        data->offset = 1068;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_PMF1_DIRECT_ACTION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_DIRECT_ACTION);
        data->offset = 944;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_STATE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_STATE_DATA);
        data->offset = 935;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_STATE_ADDRESS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_STATE_ADDRESS);
        data->offset = 925;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL);
        data->offset = 765;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_KEY_G_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_G_INITIAL);
        data->offset = 605;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_KEY_H_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_H_INITIAL);
        data->offset = 445;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_KEY_I_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_I_INITIAL);
        data->offset = 285;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_KEY_J_INITIAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_KEY_J_INITIAL);
        data->offset = 125;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_LAG_LB_KEY);
        data->offset = 109;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_NWK_LB_KEY);
        data->offset = 93;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_2);
        data->offset = 77;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_1);
        data->offset = 61;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_ECMP_LB_KEY_0);
        data->offset = 45;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_TRJ_HASH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);
        data->offset = 29;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_CMP_KEY_0_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_CMP_KEY_0_DECODED);
        data->offset = 23;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_CMP_KEY_1_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_CMP_KEY_1_DECODED);
        data->offset = 17;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_CMP_KEY_TCAM_0_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_CMP_KEY_TCAM_0_DECODED);
        data->offset = 11;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_CMP_KEY_TCAM_1_DECODED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_CMP_KEY_TCAM_1_DECODED);
        data->offset = 5;
    }
    if (DNX_FIELD_STAGE_IPMF2 < table->keys[0].size && DNX_FIELD_IPMF2_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_NASID);
        data->offset = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1369;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 436;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PP_PORT_PMF_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PP_PORT_PMF_GENERAL_DATA);
        data->offset = 372+dnx_data_field.profile_bits.nof_bits_in_port_profile_get(unit);
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 372;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_ALL_ONES < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_ALL_ONES);
        data->offset = 1698;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_EXPECTED_WON < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_EXPECTED_WON);
        data->offset = 1693;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_FOUND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_FOUND);
        data->offset = 1692;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_MATCH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_MATCH);
        data->offset = 1691;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_STATION_MOVE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_STATION_MOVE);
        data->offset = 1690;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_VLAN);
        data->offset = 1673;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_VSI);
        data->offset = 1655;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_SRC_MAC);
        data->offset = 1607;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_DATA);
        data->offset = 1541;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LEARN_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LEARN_INFO);
        data->offset = 1538;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_RPF_ECMP_MODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_ECMP_MODE);
        data->offset = 1536;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_RPF_ECMP_GROUP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_ECMP_GROUP);
        data->offset = 1520;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_ADMT_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_ADMT_PROFILE);
        data->offset = 1517;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_BIER_STR_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_BIER_STR_OFFSET);
        data->offset = 1509;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_BIER_STR_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_BIER_STR_SIZE);
        data->offset = 1507;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_BYTES_TO_REMOVE);
        data->offset = 1498;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_WEAK_TM_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_WEAK_TM_VALID);
        data->offset = 1497;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_WEAK_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_WEAK_TM_PROFILE);
        data->offset = 1491;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STAT_OBJ_LM_READ_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STAT_OBJ_LM_READ_INDEX);
        data->offset = 1489;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_CONGESTION_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_CONGESTION_INFO);
        data->offset = 1487;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_END_OF_PACKET_EDITING);
        data->offset = 1484;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_EEI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_EEI);
        data->offset = 1460;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_EGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_EGRESS_LEARN_ENABLE);
        data->offset = 1459;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_QUAL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_QUAL);
        data->offset = 1440;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_CODE);
        data->offset = 1431;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP_METER_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP_METER_COMMAND);
        data->offset = 1427;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP);
        data->offset = 1425;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_TC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_TC);
        data->offset = 1422;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DESTINATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DESTINATION);
        data->offset = 1401;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_ACTION_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_ACTION_STRENGTH);
        data->offset = 1397;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_PAYLOAD);
        data->offset = 1397;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VRF_VALUE);
        data->offset = 1379;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN);
        data->offset = 1379;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN_PROFILE);
        data->offset = 1369;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GENERAL_DATA_PMF_CONTAINER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GENERAL_DATA_PMF_CONTAINER);
        data->offset = 1337;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GENERAL_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GENERAL_DATA);
        data->offset = 1241;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IEEE1588_COMMAND < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IEEE1588_COMMAND);
        data->offset = 1239;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IEEE1588_COMPENSATE_TIME_STAMP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IEEE1588_COMPENSATE_TIME_STAMP);
        data->offset = 1238;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IEEE1588_ENCAPSULATION < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IEEE1588_ENCAPSULATION);
        data->offset = 1237;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IEEE1588_HEADER_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IEEE1588_HEADER_OFFSET);
        data->offset = 1229;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_1);
        data->offset = 1221;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_0);
        data->offset = 1213;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_1);
        data->offset = 1191;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_0);
        data->offset = 1169;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PP_PORT);
        data->offset = 1161;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_CORE_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_CORE_ID);
        data->offset = 1160;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_PORT);
        data->offset = 1160;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_TTL);
        data->offset = 1152;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_INGRESS_LEARN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_INGRESS_LEARN_ENABLE);
        data->offset = 1151;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_ITPP_DELTA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_ITPP_DELTA);
        data->offset = 1143;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LAG_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LAG_LB_KEY);
        data->offset = 1127;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID_VALID);
        data->offset = 1126;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID);
        data->offset = 1107;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_MIRROR_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_MIRROR_CODE);
        data->offset = 1098;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_MIRROR_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_MIRROR_QUALIFIER);
        data->offset = 1090;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_NWK_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_NWK_LB_KEY);
        data->offset = 1074;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_NWK_QOS);
        data->offset = 1066;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_OAM_ID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_OAM_ID);
        data->offset = 1047;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_OAM_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_OAM_OFFSET);
        data->offset = 1039;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_OAM_STAMP_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_OAM_STAMP_OFFSET);
        data->offset = 1031;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_OAM_SUB_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_OAM_SUB_TYPE);
        data->offset = 1027;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_OAM_UP_MEP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_OAM_UP_MEP);
        data->offset = 1026;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_3);
        data->offset = 1004;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_2);
        data->offset = 982;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_1);
        data->offset = 960;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_0);
        data->offset = 938;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PACKET_IS_APPLET);
        data->offset = 937;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_NASID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_NASID);
        data->offset = 932;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PACKET_IS_BIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PACKET_IS_BIER);
        data->offset = 931;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PACKET_IS_IEEE1588 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PACKET_IS_IEEE1588);
        data->offset = 930;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_EGRESS_PARSING_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_EGRESS_PARSING_INDEX);
        data->offset = 927;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PTC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PTC);
        data->offset = 919;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_EXT_STATISTICS_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_EXT_STATISTICS_VALID);
        data->offset = 918;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PPH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PPH_RESERVED);
        data->offset = 915;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PPH_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PPH_TYPE);
        data->offset = 913;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_RPF_DST < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_DST);
        data->offset = 892;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_RPF_DST_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_RPF_DST_VALID);
        data->offset = 891;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SNOOP_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SNOOP_CODE);
        data->offset = 882;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SNOOP_STRENGTH);
        data->offset = 879;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SNOOP_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SNOOP_QUALIFIER);
        data->offset = 871;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SRC_SYS_PORT);
        data->offset = 855;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_ST_VSQ_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_ST_VSQ_PTR);
        data->offset = 847;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_CODE);
        data->offset = 838;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_QUALIFIER < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_QUALIFIER);
        data->offset = 830;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STACKING_ROUTE_HISTORY_BITMAP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STACKING_ROUTE_HISTORY_BITMAP);
        data->offset = 814;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STATISTICS_META_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STATISTICS_META_DATA);
        data->offset = 798;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_STATISTICS_OBJECT10 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_STATISTICS_OBJECT10);
        data->offset = 795;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SYSTEM_HEADER_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SYSTEM_HEADER_PROFILE_INDEX);
        data->offset = 791;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_TM_PROFILE);
        data->offset = 789;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_1);
        data->offset = 757;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_2);
        data->offset = 725;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_3);
        data->offset = 693;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_4);
        data->offset = 661;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_1_TYPE);
        data->offset = 659;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_2_TYPE);
        data->offset = 657;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_3_TYPE);
        data->offset = 655;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADER_4_TYPE);
        data->offset = 653;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_USER_HEADERS_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_USER_HEADERS_TYPE);
        data->offset = 653;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VISIBILITY);
        data->offset = 652;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_VID_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_VID_2);
        data->offset = 640;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_VID_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_VID_1);
        data->offset = 628;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_DEI_2);
        data->offset = 627;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_2);
        data->offset = 624;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_DEI_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_DEI_2);
        data->offset = 624;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_DEI_1);
        data->offset = 623;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_1);
        data->offset = 620;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_DEI_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_PCP_DEI_1);
        data->offset = 620;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_CMD_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_CMD_INDEX);
        data->offset = 613;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_CMD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_CMD);
        data->offset = 613;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 609;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SLB_LEARN_NEEDED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SLB_LEARN_NEEDED);
        data->offset = 608;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SLB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SLB_KEY);
        data->offset = 528;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_SLB_PAYLOAD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_SLB_PAYLOAD);
        data->offset = 468;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_PTC_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_PTC_KEY_GEN_VAR);
        data->offset = 436;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_PORT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_PORT_KEY_GEN_VAR);
        data->offset = 372;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_IN_PORT_MAPPED_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_IN_PORT_MAPPED_PP_PORT);
        data->offset = 363;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_CONTEXT_KEY_GEN_VAR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_CONTEXT_KEY_GEN_VAR);
        data->offset = 331;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FER_STATISTICS_OBJ < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FER_STATISTICS_OBJ);
        data->offset = 259;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && DNX_FIELD_IPMF3_QUAL_FWD_LAYER_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_IPMF3_QUAL_FWD_LAYER_INDEX);
        data->offset = 256;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_TM_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_TM_PORT_PMF_PROFILE);
        data->offset = 2453;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PP_PORT_PMF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PP_PORT_PMF_PROFILE);
        data->offset = 2501;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_RESERVED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_RESERVED);
        data->offset = 1167;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_VISIBILITY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_VISIBILITY);
        data->offset = 1168;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TM_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TM_PROFILE);
        data->offset = 1169;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_ECN_ENABLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_ECN_ENABLE);
        data->offset = 1171;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_CNI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_CNI);
        data->offset = 1172;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_BIER_BFR_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_BIER_BFR_EXT_PRESENT);
        data->offset = 1173;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_FLOW_ID_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_FLOW_ID_EXT_PRESENT);
        data->offset = 1174;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT);
        data->offset = 1175;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT);
        data->offset = 1176;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR);
        data->offset = 1177;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_IS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_IS_MC);
        data->offset = 1199;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_PPH_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_PPH_PRESENT);
        data->offset = 1200;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TSH_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TSH_EXT_PRESENT);
        data->offset = 1201;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_TYPE);
        data->offset = 1202;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_DP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_DP);
        data->offset = 1204;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PP_DSP < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PP_DSP);
        data->offset = 1206;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT);
        data->offset = 1214;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_TC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_TC);
        data->offset = 1230;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FTMH_PACKET_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FTMH_PACKET_SIZE);
        data->offset = 1233;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LB_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LB_KEY);
        data->offset = 1247;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_NWK_KEY < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_NWK_KEY);
        data->offset = 1255;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_STACKING_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_STACKING_EXT);
        data->offset = 1271;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_BIER_BFR_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_BIER_BFR_EXT);
        data->offset = 1287;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_TM_DESTINATION_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_TM_DESTINATION_EXT);
        data->offset = 1303;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_APPLICATION_SPECIFIC_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_APPLICATION_SPECIFIC_EXT);
        data->offset = 1327;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_TSH_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_TSH_EXT);
        data->offset = 1375;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FWD_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FWD_STRENGTH);
        data->offset = 1431;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_TYPE);
        data->offset = 1435;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_OFFSET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_OFFSET);
        data->offset = 1440;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_LIF_EXT_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_LIF_EXT_TYPE);
        data->offset = 1447;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_FHEI_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_FHEI_SIZE);
        data->offset = 1450;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_LEARN_EXT_PRESENT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_LEARN_EXT_PRESENT);
        data->offset = 1452;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_TTL < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_TTL);
        data->offset = 1453;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_NWK_QOS < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_NWK_QOS);
        data->offset = 1461;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_IN_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_IN_LIF);
        data->offset = 1469;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_FWD_DOMAIN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_FWD_DOMAIN);
        data->offset = 1491;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_VRF_VALUE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_VRF_VALUE);
        data->offset = 1491;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_IN_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_IN_LIF_PROFILE);
        data->offset = 1509;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_END_OF_PACKET_EDITING < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_END_OF_PACKET_EDITING);
        data->offset = 1517;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO);
        data->offset = 1520;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_VALUE1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_VALUE1);
        data->offset = 1524;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PPH_VALUE2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PPH_VALUE2);
        data->offset = 1532;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FHEI_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FHEI_EXT);
        data->offset = 1540;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_EXT);
        data->offset = 1604;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_EXT_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_EXT_DATA);
        data->offset = 1607;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_EXT_SRC_MAC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_EXT_SRC_MAC);
        data->offset = 1673;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_EXT_VSI < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_EXT_VSI);
        data->offset = 1721;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_EXT_VLAN < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_EXT_VLAN);
        data->offset = 1739;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LIF_EXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LIF_EXT);
        data->offset = 1756;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_4 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_4);
        data->offset = 1828;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_3);
        data->offset = 1860;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_2);
        data->offset = 1892;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_1);
        data->offset = 1924;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_4_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_4_TYPE);
        data->offset = 1956;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_3_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_3_TYPE);
        data->offset = 1958;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_2_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_2_TYPE);
        data->offset = 1960;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_USER_HEADER_1_TYPE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_USER_HEADER_1_TYPE);
        data->offset = 1962;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IS_TDM < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IS_TDM);
        data->offset = 1964;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_CUD_OUTLIF_OR_MCDB_PTR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_CUD_OUTLIF_OR_MCDB_PTR);
        data->offset = 1965;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_DISCARD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_DISCARD);
        data->offset = 1987;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_OUT_LIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_OUT_LIF_PROFILE);
        data->offset = 1988;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PMF_ETH_RIF_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PMF_ETH_RIF_PROFILE);
        data->offset = 1991;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_ETH_TAG_FORMAT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_ETH_TAG_FORMAT);
        data->offset = 1992;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_UPDATED_TPIDS_PACKET_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_UPDATED_TPIDS_PACKET_DATA);
        data->offset = 2002;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_ADD < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_ADD);
        data->offset = 2146;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_REMOVE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_REMOVE);
        data->offset = 2151;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_ETHER_TYPE_CODE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_ETHER_TYPE_CODE);
        data->offset = 2156;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FABRIC_OR_EGRESS_MC < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FABRIC_OR_EGRESS_MC);
        data->offset = 2172;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_ESTIMATED_BTR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_ESTIMATED_BTR);
        data->offset = 2173;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FWD_CONTEXT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FWD_CONTEXT);
        data->offset = 2181;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_0);
        data->offset = 2187;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_1 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_1);
        data->offset = 2209;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_2 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_2);
        data->offset = 2231;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_3 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_3);
        data->offset = 2253;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_GLOB_IN_LIF_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_GLOB_IN_LIF_0);
        data->offset = 2275;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IN_LIF_PROFILE_0 < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IN_LIF_PROFILE_0);
        data->offset = 2297;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IP_MC_ELIGIBLE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IP_MC_ELIGIBLE);
        data->offset = 2305;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_IP_MC_SHOULD_BE_BRIDGED < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_IP_MC_SHOULD_BE_BRIDGED);
        data->offset = 2306;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LEARN_VALID < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LEARN_VALID);
        data->offset = 2307;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_LOCAL_OUT_LIF < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_LOCAL_OUT_LIF);
        data->offset = 2308;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_FWD_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_FWD_ACTION_PROFILE_INDEX);
        data->offset = 2328;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_SNOOP_ACTION_PROFILE_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_SNOOP_ACTION_PROFILE_INDEX);
        data->offset = 2331;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_SNOOP_STRENGTH < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_SNOOP_STRENGTH);
        data->offset = 2333;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_SYSTEM_HEADERS_SIZE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_SYSTEM_HEADERS_SIZE);
        data->offset = 2336;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_DST_SYS_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_DST_SYS_PORT);
        data->offset = 2343;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_OUT_TM_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_OUT_TM_PORT);
        data->offset = 2359;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_OUT_PP_PORT < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_OUT_PP_PORT);
        data->offset = 2367;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PACKET_IS_APPLET < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PACKET_IS_APPLET);
        data->offset = 2375;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_MEM_SOFT_ERR < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_MEM_SOFT_ERR);
        data->offset = 2376;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_TC_MAP_PROFILE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_TC_MAP_PROFILE);
        data->offset = 2377;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_L4_PORT_IN_RANGE < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_L4_PORT_IN_RANGE);
        data->offset = 2381;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PROGRAM_INDEX < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PROGRAM_INDEX);
        data->offset = 2405;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && DNX_FIELD_EPMF_QUAL_PER_PORT_TABLE_DATA < table->keys[1].size)
    {
        data = (dnx_data_field_qual_params_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, DNX_FIELD_EPMF_QUAL_PER_PORT_TABLE_DATA);
        data->offset = 2411;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_layer_record_info_ingress_set(
    int unit)
{
    int layer_record_index;
    dnx_data_field_qual_layer_record_info_ingress_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int table_index = dnx_data_field_qual_table_layer_record_info_ingress;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_LR_QUAL_NOF;
    table->info_get.key_size[0] = DNX_FIELD_LR_QUAL_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_qual_layer_record_info_ingress_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_qual_table_layer_record_info_ingress");

    
    default_data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->size = 0;
    default_data->offset = 0;
    
    for (layer_record_index = 0; layer_record_index < table->keys[0].size; layer_record_index++)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, layer_record_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_LR_QUAL_OFFSET < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_OFFSET, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 0;
    }
    if (DNX_FIELD_LR_QUAL_PROTOCOL < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_PROTOCOL, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 24;
    }
    if (DNX_FIELD_LR_QUAL_QUALIFIER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_QUALIFIER, 0);
        data->valid = TRUE;
        data->size = 16;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_MC, 0);
        data->valid = dnx_data_field.dnx_data_internal.lr_eth_is_da_mac_valid_get(unit);
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_BC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_BC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 14;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 19;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_ETH1 < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_ETH1, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 12;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 13;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 16;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 21;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 22;
    }
    if (DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_FIP < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_FIP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_qual_layer_record_info_egress_set(
    int unit)
{
    int layer_record_index;
    dnx_data_field_qual_layer_record_info_egress_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_qual;
    int table_index = dnx_data_field_qual_table_layer_record_info_egress;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_FIELD_LR_QUAL_NOF;
    table->info_get.key_size[0] = DNX_FIELD_LR_QUAL_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_field_qual_layer_record_info_egress_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_field_qual_table_layer_record_info_egress");

    
    default_data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->size = 0;
    default_data->offset = 0;
    
    for (layer_record_index = 0; layer_record_index < table->keys[0].size; layer_record_index++)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, layer_record_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_FIELD_LR_QUAL_OFFSET < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_OFFSET, 0);
        data->valid = TRUE;
        data->size = 7;
        data->offset = 0;
    }
    if (DNX_FIELD_LR_QUAL_PROTOCOL < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_PROTOCOL, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 23;
    }
    if (DNX_FIELD_LR_QUAL_QUALIFIER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_QUALIFIER, 0);
        data->valid = TRUE;
        data->size = 16;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_MC, 0);
        data->valid = dnx_data_field.dnx_data_internal.lr_eth_is_da_mac_valid_get(unit);
        data->size = 1;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_DA_BC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_DA_BC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 13;
    }
    if (DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 14;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX, 0);
        data->valid = TRUE;
        data->size = 3;
        data->offset = 18;
    }
    if (DNX_FIELD_LR_QUAL_ETH_IS_ETH1 < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ETH_IS_ETH1, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 21;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 10;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 11;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 12;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 16;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 17;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_IS_MC < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_IS_MC, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 14;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER, 0);
        data->valid = TRUE;
        data->size = 5;
        data->offset = 15;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 20;
    }
    if (DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 21;
    }
    if (DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE, 0);
        data->valid = TRUE;
        data->size = 8;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH, 0);
        data->valid = TRUE;
        data->size = 4;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_FIP < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_FIP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 7;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 8;
    }
    if (DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST, 0);
        data->valid = TRUE;
        data->size = 1;
        data->offset = 9;
    }
    if (DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE < table->keys[0].size)
    {
        data = (dnx_data_field_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE, 0);
        data->valid = TRUE;
        data->size = 2;
        data->offset = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


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

    
    table->values[0].default_val = "EMPTY";
    table->values[1].default_val = "DNX_FIELD_QUAL_ID_INVALID";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    
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
    default_data->signal_name = "EMPTY";
    default_data->mapped_qual = DNX_FIELD_QUAL_ID_INVALID;
    default_data->offset_from_qual = 0;
    default_data->mapped_action[0] = 0;
    default_data->action_offset[0] = 0;
    
    for (stage_index = 0; stage_index < table->keys[0].size; stage_index++)
    {
        for (signal_id_index = 0; signal_id_index < table->keys[1].size; signal_id_index++)
        {
            data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index, signal_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 0);
        data->signal_name = "mem_soft_err";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_MEM_SOFT_ERR;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 1);
        data->signal_name = "context";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 2);
        data->signal_name = "acl_context";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_ACL_CONTEXT;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 3);
        data->signal_name = "packet_is_applet";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PACKET_IS_APPLET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 4);
        data->signal_name = "general_data";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 6);
        data->signal_name = "fwd_domain_profile";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_DOMAIN_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 7);
        data->signal_name = "vlan_edit_cmd";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_VLAN_EDIT_CMD;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_SRC_SYS_PORT;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 9);
        data->signal_name = "snoop_code";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_SNOOP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 10);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 11);
        data->signal_name = "fwd_layer_additional_info";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_LAYER_ADDITIONAL_INFO;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 12);
        data->signal_name = "rpf_destination";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_RPF_DST;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 13);
        data->signal_name = "rpf_destination_valid";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_RPF_DST_VALID;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 14);
        data->signal_name = "packet_is_ieee1588";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PACKET_IS_IEEE1588;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 15);
        data->signal_name = "ieee1588_compensate_time_stamp";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IEEE1588_COMPENSATE_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 16);
        data->signal_name = "ieee1588_command";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IEEE1588_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 17);
        data->signal_name = "ieee1588_encapsulation";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IEEE1588_ENCAPSULATION;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 18);
        data->signal_name = "ieee1588_header_offset";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IEEE1588_HEADER_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 19);
        data->signal_name = "packet_is_compatible_mc";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PACKET_IS_COMPATIBLE_MC;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 20);
        data->signal_name = "prt_qualifier";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PRT_QUALIFIER;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PACKET_HEADER_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 23);
        data->signal_name = "oam_id";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_ID;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 24);
        data->signal_name = "oam_up_mep";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_UP_MEP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 25);
        data->signal_name = "oam_sub_type";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_SUB_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 26);
        data->signal_name = "oam_offset";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 27);
        data->signal_name = "oam_stamp_offset";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_STAMP_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 28);
        data->signal_name = "mirror_code";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_MIRROR_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 29);
        data->signal_name = "learn_info";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_LEARN_INFO;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 32;
        data->action_offset[2] = 64;
        data->action_offset[3] = 96;
        data->action_offset[4] = 152;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 30);
        data->signal_name = "ingress_learn_enable";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_INGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 31);
        data->signal_name = "egress_learn_enable";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_EGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 32);
        data->signal_name = "in_lifs";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_GLOB_IN_LIF_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 33);
        data->signal_name = "in_lif_profiles";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IN_LIF_PROFILE_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 38 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 38);
        data->signal_name = "fwd_action_destination";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_ACTION_DESTINATION;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 39);
        data->signal_name = "fwd_action_cpu_trap_code";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 40);
        data->signal_name = "fwd_action_cpu_trap_qual";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_ACTION_CPU_TRAP_QUAL;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 41);
        data->signal_name = "eei";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_EEI;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_EEI;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 42);
        data->signal_name = "elk_lkp_payload";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_ELK_LKP_HIT_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 43);
        data->signal_name = "in_port";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PP_PORT;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 44);
        data->signal_name = "ptc";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PTC;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PTC;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_GLOB_OUT_LIF_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 48);
        data->signal_name = "in_ttl";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_IN_TTL;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 49);
        data->signal_name = "fwd_layer_index";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_FWD_LAYER_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 50);
        data->signal_name = "saved_context_profile";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_SAVED_CONTEXT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 51);
        data->signal_name = "visibility";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_VISIBILITY;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 52);
        data->signal_name = "visibility_clear";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 55);
        data->signal_name = "rpf_out_lif";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_RPF_OUT_LIF;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 56);
        data->signal_name = "rpf_default_route_found";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_RPF_DEFAULT_ROUTE_FOUND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 57);
        data->signal_name = "rpf_route_valid";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_RPF_ROUTE_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 58);
        data->signal_name = "ecmp_lb_keys";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_ECMP_LB_KEY_0;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 59);
        data->signal_name = "lag_lb_key";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_LAG_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 60);
        data->signal_name = "nwk_lb_key";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_NWK_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 61);
        data->signal_name = "nasid";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_NASID;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_STATISTICS_META_DATA;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 65);
        data->signal_name = "oam_meter_disable";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_OAM_METER_DISABLE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 66);
        data->signal_name = "incoming_tag_structure";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_INCOMING_TAG_STRUCTURE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 67);
        data->signal_name = "tc";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_TC;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 68);
        data->signal_name = "dp";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_DP;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 69);
        data->signal_name = "ecn";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_ECN;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 70);
        data->signal_name = "nwk_qos";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_NWK_QOS;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 71);
        data->signal_name = "trace_packet";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_TRACE_PACKET;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 72);
        data->signal_name = "dp_meter_command";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 75);
        data->signal_name = "all_out_lifs";
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 77);
        data->signal_name = "stacking_route_history_bitmap";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 78);
        data->signal_name = "pph_type";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 79);
        data->signal_name = "user_header1";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 80);
        data->signal_name = "user_header2";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 81);
        data->signal_name = "user_header3";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 82);
        data->signal_name = "user_header4";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 83);
        data->signal_name = "user_headers_type";
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 88);
        data->signal_name = "pph_reserved";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 89 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 89);
        data->signal_name = "bier_string_size";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_BIER_STR_SIZE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 90);
        data->signal_name = "bier_string_offset";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_BIER_STR_OFFSET;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 91);
        data->signal_name = "egress_parsing_index";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_EGRESS_PARSING_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 92);
        data->signal_name = "congestion_info";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 93);
        data->signal_name = "statistics_objects_resolved";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_STATISTICS_ID_0;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4;
        data->mapped_action[5] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5;
        data->mapped_action[6] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6;
        data->mapped_action[7] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7;
        data->mapped_action[8] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8;
        data->mapped_action[9] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_STATISTICS_OBJECT10;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 95 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 95);
        data->signal_name = "statistics_objects_atr";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_STATISTICS_ATR_0;
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4;
        data->mapped_action[5] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5;
        data->mapped_action[6] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6;
        data->mapped_action[7] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7;
        data->mapped_action[8] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8;
        data->mapped_action[9] = DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 97);
        data->signal_name = "fwd_context";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 98 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 98);
        data->signal_name = "slb_payload";
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 32;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 99 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 99);
        data->signal_name = "slb_found";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 100 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 100);
        data->signal_name = "end_of_packet_editing";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 101);
        data->signal_name = "ext_statistics";
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_EXT_STATISTICS_ID_0;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD;
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
        data->mapped_qual = DNX_FIELD_IPMF1_QUAL_PACKET_IS_BIER;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 110 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 110);
        data->signal_name = "ingress_time_stamp";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF1 < table->keys[0].size && 111 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF1, 111);
        data->signal_name = "ingress_time_stamp_override";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_ADMT_PROFILE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 2);
        data->signal_name = "bier_string_offset";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_BIER_STR_OFFSET;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 3);
        data->signal_name = "bier_string_size";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_BIER_STR_SIZE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 4);
        data->signal_name = "congestion_info";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_CONGESTION_INFO;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 5);
        data->signal_name = "eei";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_EEI;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_EEI;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 6);
        data->signal_name = "egress_learn_enable";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_EGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 7);
        data->signal_name = "end_of_packet_editing";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_END_OF_PACKET_EDITING;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 8);
        data->signal_name = "ext_statistics_valid";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_EXT_STATISTICS_VALID;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DESTINATION;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 12);
        data->signal_name = "dp";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 13);
        data->signal_name = "tc";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_TC;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 14);
        data->signal_name = "dp_meter_command";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_DP_METER_COMMAND;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 15);
        data->signal_name = "fwd_action_strength";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 16);
        data->signal_name = "fwd_action_cpu_trap_code";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 17);
        data->signal_name = "fwd_domain_id";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 18);
        data->signal_name = "fwd_domain_profile";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_DOMAIN_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 19);
        data->signal_name = "fwd_layer_index";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_LAYER_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 20);
        data->signal_name = "general_data";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_GENERAL_DATA;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 160;
        data->action_offset[1] = 192;
        data->action_offset[2] = 254;
        data->action_offset[3] = 296;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 21);
        data->signal_name = "ieee1588_command";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IEEE1588_COMMAND;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 22);
        data->signal_name = "ieee1588_compensate_time_stamp";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IEEE1588_COMPENSATE_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 23);
        data->signal_name = "ieee1588_encapsulation";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IEEE1588_ENCAPSULATION;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 24);
        data->signal_name = "ieee1588_header_offset";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IEEE1588_HEADER_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 25);
        data->signal_name = "in_lif_profiles";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IN_LIF_PROFILE_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 8;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 26);
        data->signal_name = "in_lifs";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_GLOB_IN_LIF_0;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 22;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 27);
        data->signal_name = "in_port";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PP_PORT;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 28);
        data->signal_name = "in_ttl";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_IN_TTL;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 29);
        data->signal_name = "ingress_learn_enable";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_INGRESS_LEARN_ENABLE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 30);
        data->signal_name = "itpp_delta";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_ITPP_DELTA;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 31);
        data->signal_name = "lag_lb_key";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_LAG_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 32);
        data->signal_name = "latency_flow_id";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_MIRROR_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 39);
        data->signal_name = "mirror_qualifier";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_MIRROR_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 40);
        data->signal_name = "nwk_lb_key";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_NWK_LB_KEY;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 41);
        data->signal_name = "nwk_qos";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_NWK_QOS;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 42);
        data->signal_name = "oam_id";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_OAM_ID;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 43);
        data->signal_name = "oam_offset";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_OAM_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 44);
        data->signal_name = "oam_stamp_offset";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_OAM_STAMP_OFFSET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 45);
        data->signal_name = "oam_sub_type";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_OAM_SUB_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 46);
        data->signal_name = "oam_up_mep";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_OAM_UP_MEP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 47);
        data->signal_name = "all_out_lifs";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_GLOB_OUT_LIF_0;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PACKET_IS_APPLET;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 49);
        data->signal_name = "packet_is_bier";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PACKET_IS_BIER;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PACKET_IS_IEEE1588;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 52);
        data->signal_name = "egress_parsing_index";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_EGRESS_PARSING_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 53 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 53);
        data->signal_name = "pph_reserved";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PPH_RESERVED;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 54 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 54);
        data->signal_name = "pph_type";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PPH_TYPE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 55 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 55);
        data->signal_name = "rpf_destination";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_RPF_DST;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 56 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 56);
        data->signal_name = "rpf_destination_valid";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_RPF_DST_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 57 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 57);
        data->signal_name = "snoop_code";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SNOOP_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 58 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 58);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 59);
        data->signal_name = "snoop_qualifier";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SNOOP_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 60);
        data->signal_name = "src_system_port_id";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SRC_SYS_PORT;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 61);
        data->signal_name = "st_vsq_pointer";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_ST_VSQ_PTR;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 62);
        data->signal_name = "statistical_sampling_code";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_CODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 63);
        data->signal_name = "statistical_sampling_qualifier";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STATISTICAL_SAMPLING_QUALIFIER;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 64);
        data->signal_name = "stacking_route_history_bitmap";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STACKING_ROUTE_HISTORY_BITMAP;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 65);
        data->signal_name = "statistics_meta_data";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STATISTICS_META_DATA;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 66);
        data->signal_name = "statistics_objects_resolved";
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4;
        data->mapped_action[5] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5;
        data->mapped_action[6] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6;
        data->mapped_action[7] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7;
        data->mapped_action[8] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8;
        data->mapped_action[9] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STATISTICS_OBJECT10;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 68);
        data->signal_name = "statistics_objects_atr";
        SHR_RANGE_VERIFY(10, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4;
        data->mapped_action[5] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5;
        data->mapped_action[6] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6;
        data->mapped_action[7] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7;
        data->mapped_action[8] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8;
        data->mapped_action[9] = DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SYSTEM_HEADER_PROFILE_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 70);
        data->signal_name = "tm_profile";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_TM_PROFILE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 71);
        data->signal_name = "user_header1";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_USER_HEADER_1;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 72);
        data->signal_name = "user_header2";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_USER_HEADER_2;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 73);
        data->signal_name = "user_header3";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_USER_HEADER_3;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 74);
        data->signal_name = "user_header4";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_USER_HEADER_4;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 75);
        data->signal_name = "user_headers_type";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_USER_HEADERS_TYPE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 76);
        data->signal_name = "visibility";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_VISIBILITY;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 77);
        data->signal_name = "vlan_edit_cmd";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_VLAN_EDIT_CMD;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_LAYER_ADDITIONAL_INFO;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 79);
        data->signal_name = "slb_learn_needed";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SLB_LEARN_NEEDED;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 80 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 80);
        data->signal_name = "slb_key";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SLB_KEY;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 81 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 81);
        data->signal_name = "slb_payload";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_SLB_PAYLOAD;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 82 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 82);
        data->signal_name = "learn_info";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_LEARN_INFO;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0;
        data->mapped_action[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1;
        data->mapped_action[2] = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2;
        data->mapped_action[3] = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3;
        data->mapped_action[4] = DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT;
        SHR_RANGE_VERIFY(5, 0, DNX_DATA_MAX_FIELD_VIRTUAL_WIRE_ACTIONS_PER_SIGNAL_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->action_offset[0] = 0;
        data->action_offset[1] = 32;
        data->action_offset[2] = 64;
        data->action_offset[3] = 96;
        data->action_offset[4] = 152;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 83 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 83);
        data->signal_name = "latency_flow_id_valid";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_LATENCY_FLOW_ID_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 84 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 84);
        data->signal_name = "weak_tm_profile";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_WEAK_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 85);
        data->signal_name = "stat_obj_lm_read_index";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_STAT_OBJ_LM_READ_INDEX;
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 88 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 88);
        data->signal_name = "ingress_time_stamp_override";
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_RPF_ECMP_GROUP;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 92);
        data->signal_name = "weak_tm_valid";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_WEAK_TM_VALID;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 93);
        data->signal_name = "fer_statistics_obj";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FER_STATISTICS_OBJ;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_RPF_ECMP_MODE;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 96 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 96);
        data->signal_name = "ptc";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_PTC;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 97 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 97);
        data->signal_name = "nasid";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_NASID;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_BYTES_TO_REMOVE;
        data->offset_from_qual = 0;
    }
    if (DNX_FIELD_STAGE_IPMF3 < table->keys[0].size && 101 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_IPMF3, 101);
        data->signal_name = "bytes_to_remove_fix";
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_BYTES_TO_REMOVE;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST;
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
        data->mapped_qual = DNX_FIELD_IPMF3_QUAL_FWD_ACTION_CPU_TRAP_QUAL;
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
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_DISCARD;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 3);
        data->signal_name = "is_tdm";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IS_TDM;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 4);
        data->signal_name = "pp_dsp";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PP_DSP;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 5);
        data->signal_name = "cud_outlif_or_mcdb_ptr";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_CUD_OUTLIF_OR_MCDB_PTR;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 6 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 6);
        data->signal_name = "ftmh_packet_size";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_PACKET_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 7 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 7);
        data->signal_name = "ftmh_tc";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TC;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_TC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 8 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 8);
        data->signal_name = "ftmh_source_system_port";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_SRC_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 9 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 9);
        data->signal_name = "ftmh_dp";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_DP;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_DP;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 10 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 10);
        data->signal_name = "ftmh_tm_action_type";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_TYPE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 11 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 11);
        data->signal_name = "ftmh_tsh_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TSH_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 12 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 12);
        data->signal_name = "ftmh_pph_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_PPH_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 13 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 13);
        data->signal_name = "ftmh_tm_action_is_mc";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TM_ACTION_IS_MC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 14 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 14);
        data->signal_name = "ftmh_mcid_or_outlif_0_or_mcdb_ptr";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 15 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 15);
        data->signal_name = "ftmh_tm_destination_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 16 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 16);
        data->signal_name = "ftmh_application_specific_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 17 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 17);
        data->signal_name = "ftmh_flow_id_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_FLOW_ID_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 18 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 18);
        data->signal_name = "ftmh_bier_bfr_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_BIER_BFR_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 19 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 19);
        data->signal_name = "bier_bfr_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_BIER_BFR_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 20 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 20);
        data->signal_name = "pph_end_of_packet_editing";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_END_OF_PACKET_EDITING;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 21 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 21);
        data->signal_name = "lif_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_LIF_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 22 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 22);
        data->signal_name = "ftmh_cni";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_CNI;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 23 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 23);
        data->signal_name = "ftmh_ecn_enable";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_ECN_ENABLE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 24 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 24);
        data->signal_name = "ftmh_tm_profile";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_TM_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 25 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 25);
        data->signal_name = "ftmh_visibility";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_VISIBILITY;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 26 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 26);
        data->signal_name = "ftmh_reserved";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_RESERVED;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 27 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 27);
        data->signal_name = "lb_key_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_LB_KEY;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 28 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 28);
        data->signal_name = "stacking_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_STACKING_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 29 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 29);
        data->signal_name = "tm_destination_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_TM_DESTINATION_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 30 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 30);
        data->signal_name = "application_specific_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_APPLICATION_SPECIFIC_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 31 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 31);
        data->signal_name = "tsh_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_APPLICATION_SPECIFIC_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 32 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 32);
        data->signal_name = "pph_fhei_size";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_FHEI_SIZE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 33 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 33);
        data->signal_name = "pph_learn_ext_present";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_LEARN_EXT_PRESENT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 34 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 34);
        data->signal_name = "pph_lif_ext_type";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_LIF_EXT_TYPE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 35 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 35);
        data->signal_name = "pph_parsing_start_offset";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_OFFSET;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 36 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 36);
        data->signal_name = "pph_parsing_start_type";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_PARSING_START_TYPE;
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
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 39 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 39);
        data->signal_name = "forwarding_strength";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FWD_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 40 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 40);
        data->signal_name = "pph_in_lif_profile";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_IN_LIF_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 41 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 41);
        data->signal_name = "pph_forward_domain";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_FWD_DOMAIN;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 42 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 42);
        data->signal_name = "pph_in_lif";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_IN_LIF;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 43 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 43);
        data->signal_name = "pph_nwk_qos";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_NWK_QOS;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 44 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 44);
        data->signal_name = "pph_ttl";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_TTL;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 45 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 45);
        data->signal_name = "pph_value1";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_VALUE1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 46 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 46);
        data->signal_name = "pph_value2";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PPH_VALUE2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 47 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 47);
        data->signal_name = "fhei_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FHEI_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 48 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 48);
        data->signal_name = "learn_ext";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_LEARN_EXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 49 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 49);
        data->signal_name = "learn_valid";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_LEARN_VALID;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 50 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 50);
        data->signal_name = "udh";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_USER_HEADER_4;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 51 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 51);
        data->signal_name = "ip_mc_eligible";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IP_MC_ELIGIBLE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 52 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 52);
        data->signal_name = "system_headers_size";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_SYSTEM_HEADERS_SIZE;
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
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_MEM_SOFT_ERR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 59 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 59);
        data->signal_name = "estimated_btr";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_ESTIMATED_BTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 60 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 60);
        data->signal_name = "tc_map_profile";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_TC_MAP_PROFILE;
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 61 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 61);
        data->signal_name = "dst_sys_port";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_DST_SYS_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 62 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 62);
        data->signal_name = "out_tm_port";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_OUT_TM_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 63 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 63);
        data->signal_name = "out_pp_port";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_OUT_PP_PORT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 64 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 64);
        data->signal_name = "outlif_profile";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_OUT_LIF_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 65 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 65);
        data->signal_name = "fabric_or_egress_mc";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FABRIC_OR_EGRESS_MC;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 66 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 66);
        data->signal_name = "fwd_context";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FWD_CONTEXT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 67 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 67);
        data->signal_name = "eth_tag_format";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_ETH_TAG_FORMAT;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 68 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 68);
        data->signal_name = "updated_tpids_packet_data";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_UPDATED_TPIDS_PACKET_DATA;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 69 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 69);
        data->signal_name = "ive_bytes_to_add";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_ADD;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 70 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 70);
        data->signal_name = "ive_bytes_to_remove";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IVE_BYTES_TO_REMOVE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 71 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 71);
        data->signal_name = "eth_type_code";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_ETHER_TYPE_CODE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 72 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 72);
        data->signal_name = "inlif0";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_GLOB_IN_LIF_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 73 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 73);
        data->signal_name = "inlif_profile0";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IN_LIF_PROFILE_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 74 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 74);
        data->signal_name = "local_outlif";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_LOCAL_OUT_LIF;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 75 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 75);
        data->signal_name = "global_outlif0";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_0;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 76 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 76);
        data->signal_name = "global_outlif1";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_1;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 77 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 77);
        data->signal_name = "global_outlif2";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_2;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 78 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 78);
        data->signal_name = "global_outlif3";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_GLOB_OUT_LIF_3;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 79 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 79);
        data->signal_name = "ip_mc_should_be_bridged";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_IP_MC_SHOULD_BE_BRIDGED;
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
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_PER_PORT_TABLE_DATA;
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
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FWD_ACTION_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 85 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 85);
        data->signal_name = "snoop_action_profile_index";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_SNOOP_ACTION_PROFILE_INDEX;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 86 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 86);
        data->signal_name = "snoop_strength";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_SNOOP_STRENGTH;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 87 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 87);
        data->signal_name = "mirror_profile";
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE;
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
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 90 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 90);
        data->signal_name = "pmf_counter_0_profile";
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 91 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 91);
        data->signal_name = "pmf_counter_0_ptr";
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 92 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 92);
        data->signal_name = "pmf_counter_1_profile";
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 93 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 93);
        data->signal_name = "pmf_counter_1_ptr";
        data->mapped_action[0] = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR;
    }
    if (DNX_FIELD_STAGE_EPMF < table->keys[0].size && 94 < table->keys[1].size)
    {
        data = (dnx_data_field_virtual_wire_signal_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_FIELD_STAGE_EPMF, 94);
        data->signal_name = "packet_size";
        data->mapped_qual = DNX_FIELD_EPMF_QUAL_FTMH_PACKET_SIZE;
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
        "Configure number of in lif profile bits which will be used by PMF:\n"
        "pmf_in_lif_profile_nof_bits= 1-7\n"
        "Default: 2\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_eg_in_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_eg_in_lif;
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
        "Configure number of in-rif profile bits which will be used by PMF:\n"
        "pmf_in_lif_profile_nof_bits= 1-7\n"
        "Default: 2\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_range;
    define->property.method_str = "range";
    define->property.range_min = 1;
    define->property.range_max = 7;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_profile_bits_nof_eg_eth_rif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_nof_eg_eth_rif;
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
jr2_a0_dnx_data_field_profile_bits_pmf_sexem3_stage_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_pmf_sexem3_stage;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_STAGE_IPMF3;

    
    define->data = DNX_FIELD_STAGE_IPMF3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_SEXEM3_STAGE;
    define->property.doc = 
        "\n"
        "Set PMF stage that will use SEXEM3. It can be:\n"
        "IPMF2 - Ingress PMF stage 2.\n"
        "IPMF3 - Ingress PMF stage 3.\n"
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
jr2_a0_dnx_data_field_profile_bits_pmf_map_stage_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_profile_bits;
    int define_index = dnx_data_field_profile_bits_define_pmf_map_stage;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FIELD_STAGE_IPMF1;

    
    define->data = DNX_FIELD_STAGE_IPMF1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_PMF_MAP_STAGE;
    define->property.doc = 
        "\n"
        "Set PMF stage that will use MAP. It can be:\n"
        "IPMF1 - Ingress PMF stage 1/2.\n"
        "IPMF3 - Ingress PMF stage 3.\n"
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
jr2_a0_dnx_data_field_dir_ext_double_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_dir_ext;
    int define_index = dnx_data_field_dir_ext_define_double_key_size;
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
        "Set The size of the payload for a state table entry, indicating the number of entries.\n"
        "8 - 1K entries\n"
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
        "Set The size of the payload for a state table entry, indicating the number of entries.\n"
        "ipmf1_key_j_msb - MSB of initial key J in iPMF1\n"
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
jr2_a0_dnx_data_field_map_mdb_dt_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int feature_index = dnx_data_field_map_mdb_dt_support;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1 ;

    
    feature->data = 1 ;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_field_map_key_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_key_size;
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
jr2_a0_dnx_data_field_map_action_size_small_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_action_size_small;
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
jr2_a0_dnx_data_field_map_action_size_mid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_action_size_mid;
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
jr2_a0_dnx_data_field_map_action_size_large_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_action_size_large;
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
jr2_a0_dnx_data_field_map_ipmf1_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_ipmf1_key_select;
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
jr2_a0_dnx_data_field_map_ipmf2_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_ipmf2_key_select;
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
jr2_a0_dnx_data_field_map_ipmf3_key_select_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_map;
    int define_index = dnx_data_field_map_define_ipmf3_key_select;
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
        "Field class ID size 0 is used to determine UDH_0\n"
        "size in J1 mode and also the udh_egress_offset_0/1.\n"
        "Default: 0x0.\n"
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
        "Field class ID size 1 is used to determine UDH_1\n"
        "size in J1 mode.\n"
        "Default: 0x0.\n"
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
        "Field class ID size 2 is used to determine UDH_0\n"
        "size in J1 mode.\n"
        "Default: 0x0.\n"
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
        "Field class ID size 3 is used to determine UDH_1\n"
        "size in J1 mode.\n"
        "Default: 0x0.\n"
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
        "If set, never add learn_ext in system header (unless explictly required in FP action).\n"
        "Used in JR1 mode only. Used by DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH only.\n"
        "Default: 0x1.\n"
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
jr2_a0_dnx_data_field_exem_small_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_max_result_size;
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
jr2_a0_dnx_data_field_exem_large_max_result_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_max_result_size;
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
jr2_a0_dnx_data_field_exem_small_key_hash_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_key_hash_size;
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
jr2_a0_dnx_data_field_exem_large_key_hash_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_key_hash_size;
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
jr2_a0_dnx_data_field_exem_large_ipmf1_key_configurable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_ipmf1_key_configurable;
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
jr2_a0_dnx_data_field_exem_large_ipmf1_key_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_large_ipmf1_key;
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
jr2_a0_dnx_data_field_exem_small_nof_flush_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_exem;
    int define_index = dnx_data_field_exem_define_small_nof_flush_profiles;
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
jr2_a0_dnx_data_field_ace_key_hash_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_ace;
    int define_index = dnx_data_field_ace_define_key_hash_size;
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
jr2_a0_dnx_data_field_Compare_operand_equal_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_equal;
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
jr2_a0_dnx_data_field_Compare_operand_not_equal_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_not_equal;
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
jr2_a0_dnx_data_field_Compare_operand_smaller_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_smaller;
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
jr2_a0_dnx_data_field_Compare_operand_not_smaller_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_not_smaller;
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
jr2_a0_dnx_data_field_Compare_operand_bigger_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_bigger;
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
jr2_a0_dnx_data_field_Compare_operand_not_bigger_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_not_bigger;
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
jr2_a0_dnx_data_field_Compare_operand_nof_operands_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_Compare_operand;
    int define_index = dnx_data_field_Compare_operand_define_nof_operands;
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
jr2_a0_dnx_data_field_diag_key_signal_size_in_words_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_diag;
    int define_index = dnx_data_field_diag_define_key_signal_size_in_words;
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
jr2_a0_dnx_data_field_common_max_val_nof_keys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_keys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_keys_get(unit), dnx_data_field.base_ipmf2.nof_keys_get(unit), dnx_data_field.base_ipmf3.nof_keys_get(unit), dnx_data_field.base_epmf.nof_keys_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_keys_get(unit), dnx_data_field.base_ipmf2.nof_keys_get(unit), dnx_data_field.base_ipmf3.nof_keys_get(unit), dnx_data_field.base_epmf.nof_keys_get(unit));

    
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
jr2_a0_dnx_data_field_common_max_val_nof_program_selection_lines_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_program_selection_lines;
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
jr2_a0_dnx_data_field_common_max_val_program_selection_cam_mask_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_program_selection_cam_mask_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_ipmf2.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_ipmf3.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_epmf.program_selection_cam_mask_nof_bits_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_ipmf2.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_ipmf3.program_selection_cam_mask_nof_bits_get(unit), dnx_data_field.base_epmf.program_selection_cam_mask_nof_bits_get(unit));

    
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
jr2_a0_dnx_data_field_common_max_val_nof_80B_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_80B_zones;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_80B_zones_get(unit),dnx_data_field.base_ipmf2.nof_80B_zones_get(unit), dnx_data_field.base_ipmf3.nof_80B_zones_get(unit), dnx_data_field.base_epmf.nof_80B_zones_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_80B_zones_get(unit),dnx_data_field.base_ipmf2.nof_80B_zones_get(unit), dnx_data_field.base_ipmf3.nof_80B_zones_get(unit), dnx_data_field.base_epmf.nof_80B_zones_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_key_zones_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_key_zones;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_key_zones_get(unit), dnx_data_field.base_ipmf2.nof_key_zones_get(unit), dnx_data_field.base_ipmf3.nof_key_zones_get(unit), dnx_data_field.base_epmf.nof_key_zones_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_key_zones_get(unit), dnx_data_field.base_ipmf2.nof_key_zones_get(unit), dnx_data_field.base_ipmf3.nof_key_zones_get(unit), dnx_data_field.base_epmf.nof_key_zones_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_key_zone_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_key_zone_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_key_zone_bits_get(unit), dnx_data_field.base_ipmf2.nof_key_zone_bits_get(unit), dnx_data_field.base_ipmf3.nof_key_zone_bits_get(unit), dnx_data_field.base_epmf.nof_key_zone_bits_get(unit));

    
    define->data = UTILEX_MAX4(dnx_data_field.base_ipmf1.nof_key_zone_bits_get(unit), dnx_data_field.base_ipmf2.nof_key_zone_bits_get(unit), dnx_data_field.base_ipmf3.nof_key_zone_bits_get(unit), dnx_data_field.base_epmf.nof_key_zone_bits_get(unit));

    
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
jr2_a0_dnx_data_field_common_max_val_nof_fes_key_selects_on_one_actions_line_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_fes_key_selects_on_one_actions_line;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_fes_key_selects_on_one_actions_line_get(unit), dnx_data_field.base_ipmf3.nof_fes_key_selects_on_one_actions_line_get(unit), dnx_data_field.base_epmf.nof_fes_key_selects_on_one_actions_line_get(unit));

    
    define->data = UTILEX_MAX3(dnx_data_field.base_ipmf1.nof_fes_key_selects_on_one_actions_line_get(unit), dnx_data_field.base_ipmf3.nof_fes_key_selects_on_one_actions_line_get(unit), dnx_data_field.base_epmf.nof_fes_key_selects_on_one_actions_line_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_ffc_in_qual_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_ffc_in_qual;
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
jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_compare_pairs;
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
jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_compare_pairs_in_compare_mode_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_compare_pairs_in_compare_mode_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_field_common_max_val_nof_compare_keys_in_compare_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_common_max_val;
    int define_index = dnx_data_field_common_max_val_define_nof_compare_keys_in_compare_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_field.base_ipmf1.nof_compare_keys_in_compare_mode_get(unit);

    
    define->data = dnx_data_field.base_ipmf1.nof_compare_keys_in_compare_mode_get(unit);

    
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
jr2_a0_dnx_data_field_features_no_parser_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_field;
    int submodule_index = dnx_data_field_submodule_features;
    int define_index = dnx_data_field_features_define_no_parser_resources;
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
    data_index = dnx_data_field_base_ipmf1_define_nof_ffc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_ffc_groups_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_one_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_one_lower_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_one_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_one_upper_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_two_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_two_lower_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_two_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_two_upper_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_three_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_three_lower_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_three_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_three_upper_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_four_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_four_lower_set;
    data_index = dnx_data_field_base_ipmf1_define_ffc_group_four_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_ffc_group_four_upper_set;
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
    data_index = dnx_data_field_base_ipmf1_define_program_selection_cam_mask_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_program_selection_cam_mask_nof_bits_set;
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
    data_index = dnx_data_field_base_ipmf1_define_nof_80B_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_80B_zones_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_key_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_key_zones_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_key_zone_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_key_zone_bits_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fes_key_selects_on_one_actions_line;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fes_key_selects_on_one_actions_line_set;
    data_index = dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_min_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fem_condition_ms_bit_min_value_set;
    data_index = dnx_data_field_base_ipmf1_define_fem_condition_ms_bit_max_value;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fem_condition_ms_bit_max_value_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_programs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_programs_set;
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
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_condition;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_condition_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_condition;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_condition_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_map_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_map_index_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_map_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_map_index_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_bits_in_fem_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_bits_in_fem_id_set;
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
    data_index = dnx_data_field_base_ipmf1_define_nof_fems_per_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fems_per_context_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_fem_action_overriding_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_fem_action_overriding_bits_set;
    data_index = dnx_data_field_base_ipmf1_define_default_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_default_strength_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_compare_pairs_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_compare_pairs_in_compare_mode_set;
    data_index = dnx_data_field_base_ipmf1_define_nof_compare_keys_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_nof_compare_keys_in_compare_mode_set;
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
    data_index = dnx_data_field_base_ipmf1_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf1_define_cmp_selection;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_cmp_selection_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_instruction_size_set;
    data_index = dnx_data_field_base_ipmf1_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_ipmf1_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf1_dir_ext_single_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ipmf2;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ipmf2_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_ffc_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_ffc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_ffc_groups_set;
    data_index = dnx_data_field_base_ipmf2_define_ffc_group_one_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_ffc_group_one_lower_set;
    data_index = dnx_data_field_base_ipmf2_define_ffc_group_one_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_ffc_group_one_upper_set;
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
    data_index = dnx_data_field_base_ipmf2_define_program_selection_cam_mask_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_program_selection_cam_mask_nof_bits_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_qualifiers;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_qualifiers_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_80B_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_80B_zones_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_key_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_key_zones_set;
    data_index = dnx_data_field_base_ipmf2_define_nof_key_zone_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_nof_key_zone_bits_set;
    data_index = dnx_data_field_base_ipmf2_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf2_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf2_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf2_dir_ext_single_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ipmf3;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ipmf3_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_ffc_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_ffc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_ffc_groups_set;
    data_index = dnx_data_field_base_ipmf3_define_ffc_group_one_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_ffc_group_one_lower_set;
    data_index = dnx_data_field_base_ipmf3_define_ffc_group_one_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_ffc_group_one_upper_set;
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
    data_index = dnx_data_field_base_ipmf3_define_program_selection_cam_mask_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_program_selection_cam_mask_nof_bits_set;
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
    data_index = dnx_data_field_base_ipmf3_define_nof_80B_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_80B_zones_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_key_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_key_zones_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_key_zone_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_key_zone_bits_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_key_selects_on_one_actions_line;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_key_selects_on_one_actions_line_set;
    data_index = dnx_data_field_base_ipmf3_define_default_strength;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_default_strength_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_out_lif_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_out_lif_ranges_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_instruction_size_set;
    data_index = dnx_data_field_base_ipmf3_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_ipmf3_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_dir_ext_single_key_size_set;
    data_index = dnx_data_field_base_ipmf3_define_nof_fes_used_by_sdk;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ipmf3_nof_fes_used_by_sdk_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_epmf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_epmf_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_ffc_set;
    data_index = dnx_data_field_base_epmf_define_nof_ffc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_ffc_groups_set;
    data_index = dnx_data_field_base_epmf_define_ffc_group_one_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_ffc_group_one_lower_set;
    data_index = dnx_data_field_base_epmf_define_ffc_group_one_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_ffc_group_one_upper_set;
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
    data_index = dnx_data_field_base_epmf_define_program_selection_cam_mask_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_program_selection_cam_mask_nof_bits_set;
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
    data_index = dnx_data_field_base_epmf_define_nof_80B_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_80B_zones_set;
    data_index = dnx_data_field_base_epmf_define_nof_key_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_key_zones_set;
    data_index = dnx_data_field_base_epmf_define_nof_key_zone_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_key_zone_bits_set;
    data_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_base_epmf_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_base_epmf_define_nof_fes_key_selects_on_one_actions_line;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_fes_key_selects_on_one_actions_line_set;
    data_index = dnx_data_field_base_epmf_define_nof_l4_ops_ranges_legacy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_nof_l4_ops_ranges_legacy_set;
    data_index = dnx_data_field_base_epmf_define_fes_key_select_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_key_select_for_zero_bit_set;
    data_index = dnx_data_field_base_epmf_define_fes_shift_for_zero_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_shift_for_zero_bit_set;
    data_index = dnx_data_field_base_epmf_define_fes_instruction_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_instruction_size_set;
    data_index = dnx_data_field_base_epmf_define_fes_pgm_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_fes_pgm_id_offset_set;
    data_index = dnx_data_field_base_epmf_define_dir_ext_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_epmf_dir_ext_single_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_base_ifwd2;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_base_ifwd2_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_ffc_set;
    data_index = dnx_data_field_base_ifwd2_define_nof_ffc_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_nof_ffc_groups_set;
    data_index = dnx_data_field_base_ifwd2_define_ffc_group_one_lower;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_ffc_group_one_lower_set;
    data_index = dnx_data_field_base_ifwd2_define_ffc_group_one_upper;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_base_ifwd2_ffc_group_one_upper_set;
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
    
    submodule_index = dnx_data_field_submodule_kbp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_kbp_define_nof_fgs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_nof_fgs_set;
    data_index = dnx_data_field_kbp_define_max_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_max_single_key_size_set;
    data_index = dnx_data_field_kbp_define_nof_acl_keys_master_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_nof_acl_keys_master_max_set;
    data_index = dnx_data_field_kbp_define_nof_acl_keys_fg_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_nof_acl_keys_fg_max_set;
    data_index = dnx_data_field_kbp_define_min_acl_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_min_acl_nof_ffc_set;
    data_index = dnx_data_field_kbp_define_max_fwd_context_num_for_one_apptype;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_max_fwd_context_num_for_one_apptype_set;
    data_index = dnx_data_field_kbp_define_max_acl_context_num;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_max_acl_context_num_set;
    data_index = dnx_data_field_kbp_define_size_apptype_profile_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_size_apptype_profile_id_set;
    data_index = dnx_data_field_kbp_define_key_bmp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_key_bmp_set;
    data_index = dnx_data_field_kbp_define_apptype_user_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_apptype_user_1st_set;
    data_index = dnx_data_field_kbp_define_apptype_user_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_apptype_user_nof_set;
    data_index = dnx_data_field_kbp_define_max_payload_size_per_opcode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_kbp_max_payload_size_per_opcode_set;

    

    
    data_index = dnx_data_field_kbp_table_ffc_to_quad_and_group_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_kbp_ffc_to_quad_and_group_map_set;
    
    submodule_index = dnx_data_field_submodule_tcam;
    submodule = &module->submodules[submodule_index];

    
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
    data_index = dnx_data_field_tcam_define_entry_size_single_valid_bits_hw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_entry_size_single_valid_bits_hw_set;
    data_index = dnx_data_field_tcam_define_entry_size_half_payload_hw;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_entry_size_half_payload_hw_set;
    data_index = dnx_data_field_tcam_define_hw_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_hw_bank_size_set;
    data_index = dnx_data_field_tcam_define_small_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_small_bank_size_set;
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
    data_index = dnx_data_field_tcam_define_cascaded_data_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_cascaded_data_nof_bits_set;
    data_index = dnx_data_field_tcam_define_big_bank_key_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_big_bank_key_nof_bits_set;
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
    data_index = dnx_data_field_tcam_define_nof_keys_in_double_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_tcam_nof_keys_in_double_key_set;
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

    
    data_index = dnx_data_field_tcam_tcam_320b_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_tcam_tcam_320b_support_set;

    
    
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

    

    
    data_index = dnx_data_field_efes_table_key_select_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_efes_key_select_properties_set;
    
    submodule_index = dnx_data_field_submodule_fem;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_fem_define_max_nof_key_selects_per_field_io;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_fem_max_nof_key_selects_per_field_io_set;

    

    
    data_index = dnx_data_field_fem_table_key_select_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_fem_key_select_properties_set;
    
    submodule_index = dnx_data_field_submodule_context;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_context_define_default_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_context_set;
    data_index = dnx_data_field_context_define_default_itmh_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_context_set;
    data_index = dnx_data_field_context_define_default_itmh_pph_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_itmh_pph_context_set;
    data_index = dnx_data_field_context_define_default_j1_itmh_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_j1_itmh_context_set;
    data_index = dnx_data_field_context_define_default_j1_itmh_pph_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_j1_itmh_pph_context_set;
    data_index = dnx_data_field_context_define_default_stacking_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_stacking_context_set;
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
    data_index = dnx_data_field_context_define_default_nat_context;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_default_nat_context_set;
    data_index = dnx_data_field_context_define_nof_hash_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_context_nof_hash_keys_set;

    

    
    
    submodule_index = dnx_data_field_submodule_preselector;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_preselector_define_default_oam_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_oam_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_oamp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_oamp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_oamp_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_oamp_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_stacking_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_stacking_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_1st_pass_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_1st_pass_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_j1_learn_presel_id_2nd_pass_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_learn_presel_id_2nd_pass_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_nat_presel_id_ipmf1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_nat_presel_id_ipmf1_set;
    data_index = dnx_data_field_preselector_define_default_itmh_pph_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_itmh_pph_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_itmh_pph_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_itmh_pph_presel_id_ipmf3_set;
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
    data_index = dnx_data_field_preselector_define_default_j1_php_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_php_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_swap_presel_id_ipmf3;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_swap_presel_id_ipmf3_set;
    data_index = dnx_data_field_preselector_define_default_j1_same_port_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_j1_same_port_presel_id_epmf_set;
    data_index = dnx_data_field_preselector_define_default_learn_limit_presel_id_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_preselector_default_learn_limit_presel_id_epmf_set;
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
    data_index = dnx_data_field_qual_define_ac_lif_wide_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_qual_ac_lif_wide_size_set;

    

    
    data_index = dnx_data_field_qual_table_params;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_qual_params_set;
    data_index = dnx_data_field_qual_table_layer_record_info_ingress;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_qual_layer_record_info_ingress_set;
    data_index = dnx_data_field_qual_table_layer_record_info_egress;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_field_qual_layer_record_info_egress_set;
    
    submodule_index = dnx_data_field_submodule_action;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_action_define_user_1st;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_user_1st_set;
    data_index = dnx_data_field_action_define_user_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_action_user_nof_set;
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

    
    data_index = dnx_data_field_profile_bits_define_nof_ing_in_lif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_ing_in_lif_set;
    data_index = dnx_data_field_profile_bits_define_nof_eg_in_lif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_eg_in_lif_set;
    data_index = dnx_data_field_profile_bits_define_nof_ing_eth_rif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_ing_eth_rif_set;
    data_index = dnx_data_field_profile_bits_define_nof_eg_eth_rif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_nof_eg_eth_rif_set;
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
    data_index = dnx_data_field_profile_bits_define_pmf_sexem3_stage;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_pmf_sexem3_stage_set;
    data_index = dnx_data_field_profile_bits_define_pmf_map_stage;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_profile_bits_pmf_map_stage_set;

    

    
    
    submodule_index = dnx_data_field_submodule_dir_ext;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_dir_ext_define_half_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dir_ext_half_key_size_set;
    data_index = dnx_data_field_dir_ext_define_single_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dir_ext_single_key_size_set;
    data_index = dnx_data_field_dir_ext_define_double_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_dir_ext_double_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_state_table;
    submodule = &module->submodules[submodule_index];

    
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

    

    
    
    submodule_index = dnx_data_field_submodule_map;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_map_define_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_key_size_set;
    data_index = dnx_data_field_map_define_action_size_small;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_action_size_small_set;
    data_index = dnx_data_field_map_define_action_size_mid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_action_size_mid_set;
    data_index = dnx_data_field_map_define_action_size_large;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_action_size_large_set;
    data_index = dnx_data_field_map_define_ipmf1_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_ipmf1_key_select_set;
    data_index = dnx_data_field_map_define_ipmf2_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_ipmf2_key_select_set;
    data_index = dnx_data_field_map_define_ipmf3_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_map_ipmf3_key_select_set;

    
    data_index = dnx_data_field_map_mdb_dt_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_field_map_mdb_dt_support_set;

    
    
    submodule_index = dnx_data_field_submodule_hash;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_hash_define_max_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_hash_max_key_size_set;

    

    
    
    submodule_index = dnx_data_field_submodule_udh;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_udh_define_type_count;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_count_set;
    data_index = dnx_data_field_udh_define_type_0_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_0_length_set;
    data_index = dnx_data_field_udh_define_type_1_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_1_length_set;
    data_index = dnx_data_field_udh_define_type_2_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_2_length_set;
    data_index = dnx_data_field_udh_define_type_3_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_udh_type_3_length_set;
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
    data_index = dnx_data_field_exem_define_small_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_max_result_size_set;
    data_index = dnx_data_field_exem_define_large_max_result_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_max_result_size_set;
    data_index = dnx_data_field_exem_define_small_max_container_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_max_container_size_set;
    data_index = dnx_data_field_exem_define_large_max_container_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_max_container_size_set;
    data_index = dnx_data_field_exem_define_small_key_hash_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_key_hash_size_set;
    data_index = dnx_data_field_exem_define_large_key_hash_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_key_hash_size_set;
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
    data_index = dnx_data_field_exem_define_large_ipmf1_key_configurable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_ipmf1_key_configurable_set;
    data_index = dnx_data_field_exem_define_large_ipmf1_key;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_large_ipmf1_key_set;
    data_index = dnx_data_field_exem_define_small_nof_flush_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_exem_small_nof_flush_profiles_set;

    

    
    
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

    
    data_index = dnx_data_field_ace_define_ace_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_ace_id_size_set;
    data_index = dnx_data_field_ace_define_key_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_key_size_set;
    data_index = dnx_data_field_ace_define_key_hash_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_ace_key_hash_size_set;
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

    

    
    
    submodule_index = dnx_data_field_submodule_Compare_operand;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_Compare_operand_define_equal;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_equal_set;
    data_index = dnx_data_field_Compare_operand_define_not_equal;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_not_equal_set;
    data_index = dnx_data_field_Compare_operand_define_smaller;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_smaller_set;
    data_index = dnx_data_field_Compare_operand_define_not_smaller;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_not_smaller_set;
    data_index = dnx_data_field_Compare_operand_define_bigger;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_bigger_set;
    data_index = dnx_data_field_Compare_operand_define_not_bigger;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_not_bigger_set;
    data_index = dnx_data_field_Compare_operand_define_nof_operands;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_Compare_operand_nof_operands_set;

    

    
    
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
    data_index = dnx_data_field_diag_define_key_signal_size_in_words;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_diag_key_signal_size_in_words_set;
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

    
    
    submodule_index = dnx_data_field_submodule_common_max_val;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_field_common_max_val_define_nof_ffc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_ffc_set;
    data_index = dnx_data_field_common_max_val_define_nof_keys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_keys_set;
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
    data_index = dnx_data_field_common_max_val_define_nof_program_selection_lines;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_program_selection_lines_set;
    data_index = dnx_data_field_common_max_val_define_program_selection_cam_mask_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_program_selection_cam_mask_nof_bits_set;
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
    data_index = dnx_data_field_common_max_val_define_nof_80B_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_80B_zones_set;
    data_index = dnx_data_field_common_max_val_define_nof_key_zones;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_key_zones_set;
    data_index = dnx_data_field_common_max_val_define_nof_key_zone_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_key_zone_bits_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_action_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fes_key_select;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fes_key_select_set;
    data_index = dnx_data_field_common_max_val_define_nof_fes_key_selects_on_one_actions_line;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fes_key_selects_on_one_actions_line_set;
    data_index = dnx_data_field_common_max_val_define_nof_ffc_in_qual;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_ffc_in_qual_set;
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
    data_index = dnx_data_field_common_max_val_define_nof_compare_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_set;
    data_index = dnx_data_field_common_max_val_define_nof_compare_pairs_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_compare_pairs_in_compare_mode_set;
    data_index = dnx_data_field_common_max_val_define_nof_compare_keys_in_compare_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_compare_keys_in_compare_mode_set;
    data_index = dnx_data_field_common_max_val_define_nof_bits_in_fem_action;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_bits_in_fem_action_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_condition;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_condition_set;
    data_index = dnx_data_field_common_max_val_define_nof_fem_map_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_common_max_val_nof_fem_map_index_set;
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
    data_index = dnx_data_field_init_define_flow_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_flow_id_set;
    data_index = dnx_data_field_init_define_roo;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_init_roo_set;
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

    

    
    
    submodule_index = dnx_data_field_submodule_features;
    submodule = &module->submodules[submodule_index];

    
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
    data_index = dnx_data_field_features_define_no_parser_resources;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_no_parser_resources_set;
    data_index = dnx_data_field_features_define_dir_ext_epmf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_dir_ext_epmf_set;
    data_index = dnx_data_field_features_define_hashing_process_lsb_to_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_hashing_process_lsb_to_msb_set;
    data_index = dnx_data_field_features_define_exem_age_flush_scan;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_field_features_exem_age_flush_scan_set;

    

    
    
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

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

