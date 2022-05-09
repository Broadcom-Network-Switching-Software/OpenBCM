
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/l3/l3_vrrp.h>








static shr_error_e
jr2_a0_dnx_data_l3_egr_pointed_first_egr_pointed_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_egr_pointed;
    int define_index = dnx_data_l3_egr_pointed_define_first_egr_pointed_id;
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
jr2_a0_dnx_data_l3_egr_pointed_nof_egr_pointed_ids_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_egr_pointed;
    int define_index = dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_l3.egr_pointed.nof_bits_in_egr_pointed_id_get(unit);

    
    define->data = 1 << dnx_data_l3.egr_pointed.nof_bits_in_egr_pointed_id_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_egr_pointed;
    int define_index = dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);

    
    define->data = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_l3_fec_uneven_bank_sizes_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int feature_index = dnx_data_l3_fec_uneven_bank_sizes;
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
jr2_a0_dnx_data_l3_fec_mc_rpf_sip_based_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int feature_index = dnx_data_l3_fec_mc_rpf_sip_based_supported;
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
jr2_a0_dnx_data_l3_fec_nof_fecs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_fecs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xC0000;

    
    define->data = 0xC0000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_first_valid_fec_ecmp_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_first_valid_fec_ecmp_id;
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
jr2_a0_dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MIN(dnx_data_l3.fec.nof_fecs_get(unit),0x80000)-1;

    
    define->data = UTILEX_MIN(dnx_data_l3.fec.nof_fecs_get(unit),0x80000)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.fec.nof_fecs_get(unit)-1;

    
    define->data = dnx_data_l3.fec.nof_fecs_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_max_default_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_default_fec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1FFFF;

    
    define->data = 0x1FFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 26214;

    
    define->data = 26214;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_bank_size_round_up_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_bank_size_round_up;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.fec.bank_size_get(unit)+2;

    
    define->data = dnx_data_l3.fec.bank_size_get(unit)+2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_max_nof_super_fecs_per_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_nof_super_fecs_per_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.fec.bank_size_round_up_get(unit)/2;

    
    define->data = dnx_data_l3.fec.bank_size_round_up_get(unit)/2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_nof_physical_fecs_per_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_physical_fecs_per_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8192;

    
    define->data = 8192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_bank_nof_physical_rows_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_bank_nof_physical_rows;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x2000;

    
    define->data = 0x2000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_super_fec_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_super_fec_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 150;

    
    define->data = 150;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_max_nof_banks_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_l3.fec.nof_fecs_get(unit)/dnx_data_l3.fec.bank_size_get(unit));

    
    define->data = (dnx_data_l3.fec.nof_fecs_get(unit)/dnx_data_l3.fec.bank_size_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_first_bank_without_id_alloc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_first_bank_without_id_alloc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_l3.ecmp.total_nof_ecmp_get(unit) + dnx_data_l3.fec.bank_size_get(unit) - 1)/dnx_data_l3.fec.bank_size_get(unit);

    
    define->data = (dnx_data_l3.ecmp.total_nof_ecmp_get(unit) + dnx_data_l3.fec.bank_size_get(unit) - 1)/dnx_data_l3.fec.bank_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_first_shared_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_first_shared_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_l3.fec.first_bank_without_id_alloc_get(unit) - 1);

    
    define->data = (dnx_data_l3.fec.first_bank_without_id_alloc_get(unit) - 1);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_max_super_fec_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_max_super_fec_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_l3.fec.nof_fecs_get(unit)/2-1);

    
    define->data = (dnx_data_l3.fec.nof_fecs_get(unit)/2-1);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_nof_fec_directions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_fec_directions;
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
jr2_a0_dnx_data_l3_fec_nof_dpc_fec_db_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_dpc_fec_db;
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
jr2_a0_dnx_data_l3_fec_nof_fec_dbs_all_cores_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_fec_dbs_all_cores;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (dnx_data_mdb.direct.nof_fec_dbs_get(unit) - dnx_data_l3.fec.nof_dpc_fec_db_get(unit) + (dnx_data_l3.fec.nof_dpc_fec_db_get(unit) * dnx_data_device.general.nof_cores_get(unit)));

    
    define->data = (dnx_data_mdb.direct.nof_fec_dbs_get(unit) - dnx_data_l3.fec.nof_dpc_fec_db_get(unit) + (dnx_data_l3.fec.nof_dpc_fec_db_get(unit) * dnx_data_device.general.nof_cores_get(unit)));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_nof_fec_result_types_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_nof_fec_result_types;
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
jr2_a0_dnx_data_l3_fec_fer_hw_version_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_fer_hw_version;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_L3_FER_HW_VERSION_1;

    
    define->data = DNX_L3_FER_HW_VERSION_1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_property_supported_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_fec_property_supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY | BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY | BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY);

    
    define->data = (BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY | BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY | BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_supported_fec_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_supported_fec_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY | BCM_L3_MC_NO_RPF | BCM_L3_MC_RPF_EXPLICIT | BCM_L3_MC_RPF_EXPLICIT_ECMP | BCM_L3_MC_RPF_SIP_BASE | BCM_L3_ENCAP_SPACE_OPTIMIZED);

    
    define->data = (BCM_L3_2ND_HIERARCHY | BCM_L3_3RD_HIERARCHY | BCM_L3_MC_NO_RPF | BCM_L3_MC_RPF_EXPLICIT | BCM_L3_MC_RPF_EXPLICIT_ECMP | BCM_L3_MC_RPF_SIP_BASE | BCM_L3_ENCAP_SPACE_OPTIMIZED);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_supported_fec_flags2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_supported_fec_flags2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP);

    
    define->data = (BCM_L3_FLAGS2_ECMP_RANGE_OVERLAP);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_resource_info_set(
    int unit)
{
    int resource_type_index;
    dnx_data_l3_fec_fec_resource_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_fec_resource_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_fec_resource_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fec_table_fec_resource_info");

    
    default_data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_protection_type = 0;
    default_data->is_statistic_type = 0;
    default_data->nof_statistics = 0;
    default_data->dbal_result_type = 0;
    
    for (resource_type_index = 0; resource_type_index < table->keys[0].size; resource_type_index++)
    {
        data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, resource_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT, 0);
        data->is_protection_type = 0;
        data->is_statistic_type = 0;
        data->nof_statistics = 0;
        data->dbal_result_type = DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION;
    }
    if (DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT, 0);
        data->is_protection_type = 1;
        data->is_statistic_type = 1;
        data->nof_statistics = 1;
        data->dbal_result_type = DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION_W_1_STAT;
    }
    if (DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT, 0);
        data->is_protection_type = 1;
        data->is_statistic_type = 0;
        data->nof_statistics = 0;
        data->dbal_result_type = DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION;
    }
    if (DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_W_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_resource_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_W_STAT, 0);
        data->is_protection_type = 0;
        data->is_statistic_type = 1;
        data->nof_statistics = 2;
        data->dbal_result_type = DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION_W_2_STAT;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_super_fec_result_types_map_set(
    int unit)
{
    int dbal_result_type_index;
    dnx_data_l3_fec_super_fec_result_types_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_super_fec_result_types_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_super_fec_result_types_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fec_table_super_fec_result_types_map");

    
    default_data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fec_type_in_super_fec = 0;
    default_data->fec_resource_type = 0;
    
    for (dbal_result_type_index = 0; dbal_result_type_index < table->keys[0].size; dbal_result_type_index++)
    {
        data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, dbal_result_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION, 0);
        data->fec_type_in_super_fec = DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_NO_STATS;
        data->fec_resource_type = DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT;
    }
    if (DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION_W_1_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION_W_1_STAT, 0);
        data->fec_type_in_super_fec = DBAL_FIELD_FEC_ENTRY_W_PROTECTION_W_1_STATS;
        data->fec_resource_type = DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_W_STAT;
    }
    if (DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_W_PROTECTION, 0);
        data->fec_type_in_super_fec = DBAL_FIELD_FEC_ENTRY_W_PROTECTION_NO_STATS;
        data->fec_resource_type = DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_W_PROT_NO_STAT;
    }
    if (DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION_W_2_STAT < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_super_fec_result_types_map_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_RESULT_TYPE_SUPER_FEC_1ST_HIERARCHY_SUPER_FEC_NO_PROTECTION_W_2_STAT, 0);
        data->fec_type_in_super_fec = DBAL_FIELD_FEC_ENTRY_NO_PROTECTION_W_2_STATS;
        data->fec_resource_type = DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_W_STAT;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_result_type_fields_set(
    int unit)
{
    int result_type_index;
    dnx_data_l3_fec_fec_result_type_fields_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_fec_result_type_fields;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_fec_result_type_fields_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fec_table_fec_result_type_fields");

    
    default_data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->has_global_outlif = 0;
    default_data->has_global_outlif_second = 0;
    default_data->has_mc_rpf = 0;
    default_data->has_eei = 0;
    default_data->has_htm = 0;
    
    for (result_type_index = 0; result_type_index < table->keys[0].size; result_type_index++)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, result_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION, 0);
        data->has_mc_rpf = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_EEI < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_EEI, 0);
        data->has_eei = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0 < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0, 0);
        data->has_global_outlif = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1 < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_17BIT_LIF1, 0);
        data->has_global_outlif = 1;
        data->has_global_outlif_second = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_HTM < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_HTM, 0);
        data->has_global_outlif = 1;
        data->has_htm = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_LIF1_MC_RPF, 0);
        data->has_global_outlif = 1;
        data->has_global_outlif_second = 1;
        data->has_mc_rpf = 1;
    }
    if (DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_MC_RPF < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_result_type_fields_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_IRPP_FEC_ENTRY_FORMAT_FEC_ENTRY_DESTINATION_LIF0_MC_RPF, 0);
        data->has_global_outlif = 1;
        data->has_mc_rpf = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_physical_db_set(
    int unit)
{
    dnx_data_l3_fec_fec_physical_db_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_fec_physical_db;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_NOF_PHYSICAL_TABLES";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_fec_physical_db_t, (1 + 1  ), "data of dnx_data_l3_fec_table_fec_physical_db");

    
    default_data = (dnx_data_l3_fec_fec_physical_db_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->physical_table[0] = MDB_NOF_PHYSICAL_TABLES;
    
    data = (dnx_data_l3_fec_fec_physical_db_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    data = (dnx_data_l3_fec_fec_physical_db_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    data->physical_table[0] = MDB_PHYSICAL_TABLE_FEC_1;
    data->physical_table[1] = MDB_PHYSICAL_TABLE_FEC_2;
    data->physical_table[2] = MDB_PHYSICAL_TABLE_FEC_3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_tables_info_set(
    int unit)
{
    int mdb_table_index;
    dnx_data_l3_fec_fec_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_fec_tables_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = MDB_NOF_PHYSICAL_TABLES;
    table->info_get.key_size[0] = MDB_NOF_PHYSICAL_TABLES;

    
    table->values[0].default_val = "DBAL_NOF_TABLES";
    table->values[1].default_val = "DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_fec_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fec_table_fec_tables_info");

    
    default_data = (dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_NOF_TABLES;
    default_data->hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY;
    default_data->index = 0;
    
    for (mdb_table_index = 0; mdb_table_index < table->keys[0].size; mdb_table_index++)
    {
        data = (dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, mdb_table_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (MDB_PHYSICAL_TABLE_FEC_1 < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_1, 0);
        data->dbal_table = DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY;
        data->hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
        data->index = 0;
    }
    if (MDB_PHYSICAL_TABLE_FEC_2 < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_2, 0);
        data->dbal_table = DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY;
        data->hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2;
        data->index = 1;
    }
    if (MDB_PHYSICAL_TABLE_FEC_3 < table->keys[0].size)
    {
        data = (dnx_data_l3_fec_fec_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, MDB_PHYSICAL_TABLE_FEC_3, 0);
        data->dbal_table = DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY;
        data->hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3;
        data->index = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fec_fec_hierachy_position_in_range_set(
    int unit)
{
    int hierarchy_index;
    dnx_data_l3_fec_fec_hierachy_position_in_range_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int table_index = dnx_data_l3_fec_table_fec_hierachy_position_in_range;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES + 1;
    table->info_get.key_size[0] = DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES + 1;

    
    table->values[0].default_val = "DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fec_fec_hierachy_position_in_range_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fec_table_fec_hierachy_position_in_range");

    
    default_data = (dnx_data_l3_fec_fec_hierachy_position_in_range_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->forced_position = DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES;
    
    for (hierarchy_index = 0; hierarchy_index < table->keys[0].size; hierarchy_index++)
    {
        data = (dnx_data_l3_fec_fec_hierachy_position_in_range_t *) dnxc_data_mgmt_table_data_get(unit, table, hierarchy_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_FEC_ID_RANGE_POSITION;
    table->values[0].property.doc =
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_direct_map;
    table->values[0].property.method_str = "suffix_direct_map";
    table->values[0].property.suffix = "";
    table->values[0].property.nof_mapping = 3;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_l3_fec_fec_hierachy_position_in_range_t property mapping");

    table->values[0].property.mapping[0].name = "LOW";
    table->values[0].property.mapping[0].val = 0;
    table->values[0].property.mapping[1].name = "MID";
    table->values[0].property.mapping[1].val = 1;
    table->values[0].property.mapping[2].name = "HIGH";
    table->values[0].property.mapping[2].val = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (hierarchy_index = 0; hierarchy_index < table->keys[0].size; hierarchy_index++)
    {
        data = (dnx_data_l3_fec_fec_hierachy_position_in_range_t *) dnxc_data_mgmt_table_data_get(unit, table, hierarchy_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, hierarchy_index, &data->forced_position));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l3_fer_mux_connection_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fer;
    int feature_index = dnx_data_l3_fer_mux_connection_supported;
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
jr2_a0_dnx_data_l3_fer_mdb_granularity_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fer;
    int define_index = dnx_data_l3_fer_define_mdb_granularity_ratio;
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
jr2_a0_dnx_data_l3_source_address_source_address_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int define_index = dnx_data_l3_source_address_define_source_address_table_size;
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
jr2_a0_dnx_data_l3_source_address_nof_mymac_prefixes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int define_index = dnx_data_l3_source_address_define_nof_mymac_prefixes;
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
jr2_a0_dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int define_index = dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp;
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
jr2_a0_dnx_data_l3_source_address_custom_sa_use_dual_homing_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int define_index = dnx_data_l3_source_address_define_custom_sa_use_dual_homing;
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
jr2_a0_dnx_data_l3_source_address_address_type_info_set(
    int unit)
{
    int address_type_index;
    dnx_data_l3_source_address_address_type_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address;
    int table_index = dnx_data_l3_source_address_table_address_type_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = source_address_type_count;
    table->info_get.key_size[0] = source_address_type_count;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_source_address_address_type_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_source_address_table_address_type_info");

    
    default_data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_entries = 0;
    
    for (address_type_index = 0; address_type_index < table->keys[0].size; address_type_index++)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, address_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (source_address_type_default < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_default, 0);
        data->nof_entries = 1;
    }
    if (source_address_type_mac_prefix < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_mac_prefix, 0);
        data->nof_entries = 2;
    }
    if (source_address_type_ipv4 < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_ipv4, 0);
        data->nof_entries = 1;
    }
    if (source_address_type_ipv6 < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_ipv6, 0);
        data->nof_entries = 4;
    }
    if (source_address_type_mac_dual_homing < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_mac_dual_homing, 0);
        data->nof_entries = 2;
    }
    if (source_address_type_full_mac < table->keys[0].size)
    {
        data = (dnx_data_l3_source_address_address_type_info_t *) dnxc_data_mgmt_table_data_get(unit, table, source_address_type_full_mac, 0);
        data->nof_entries = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_l3_vrf_nof_vrf_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrf;
    int define_index = dnx_data_l3_vrf_define_nof_vrf;
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
jr2_a0_dnx_data_l3_vrf_nof_vrf_ipv6_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrf;
    int define_index = dnx_data_l3_vrf_define_nof_vrf_ipv6;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16384;

    
    define->data = 16384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int feature_index = dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support;
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
jr2_a0_dnx_data_l3_routing_enablers_routing_enable_vector_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enable_vector_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.routing_enablers.routing_enable_indication_nof_bits_per_layer_get(unit) * dnx_data_l3.routing_enablers.routing_enable_nof_profiles_per_layer_get(unit) * dnx_data_device.general.max_nof_layer_protocols_get(unit);

    
    define->data = dnx_data_l3.routing_enablers.routing_enable_indication_nof_bits_per_layer_get(unit) * dnx_data_l3.routing_enablers.routing_enable_nof_profiles_per_layer_get(unit) * dnx_data_device.general.max_nof_layer_protocols_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_routing_enablers_routing_enable_action_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enable_action_support;
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
jr2_a0_dnx_data_l3_routing_enablers_routing_enable_action_profile_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size;
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
jr2_a0_dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer;
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
jr2_a0_dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer;
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
jr2_a0_dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id;
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
jr2_a0_dnx_data_l3_routing_enablers_layer_enablers_by_id_set(
    int unit)
{
    int idx_index;
    dnx_data_l3_routing_enablers_layer_enablers_by_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int table_index = dnx_data_l3_routing_enablers_table_layer_enablers_by_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.max_nof_layer_protocols_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.max_nof_layer_protocols_get(unit);

    
    table->values[0].default_val = "DBAL_NOF_ENUM_LAYER_TYPES_VALUES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_routing_enablers_layer_enablers_by_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_routing_enablers_table_layer_enablers_by_id");

    
    default_data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->layer_type = DBAL_NOF_ENUM_LAYER_TYPES_VALUES;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV4;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPV6;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_MPLS;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_FCOE;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_L2TP;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_GTP;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_PPP;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_by_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->layer_type = DBAL_ENUM_FVAL_LAYER_TYPES_IPVX;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_routing_enablers_layer_enablers_set(
    int unit)
{
    int layer_type_index;
    dnx_data_l3_routing_enablers_layer_enablers_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int table_index = dnx_data_l3_routing_enablers_table_layer_enablers;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_LAYER_TYPES_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_LAYER_TYPES_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_routing_enablers_layer_enablers_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_routing_enablers_table_layer_enablers");

    
    default_data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->uc_enable = 0;
    default_data->mc_enable = 0;
    default_data->disable_profile = 0;
    
    for (layer_type_index = 0; layer_type_index < table->keys[0].size; layer_type_index++)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, layer_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_IPV4 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_IPV4, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_IPV6 < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_IPV6, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_MPLS < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_FCOE < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_FCOE, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_L2TP < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_L2TP, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_INGRESS_SCTP_EGRESS_FTMH, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_GTP < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_GTP, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_PPP < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_PPP, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }
    if (DBAL_ENUM_FVAL_LAYER_TYPES_IPVX < table->keys[0].size)
    {
        data = (dnx_data_l3_routing_enablers_layer_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_LAYER_TYPES_IPVX, 0);
        data->uc_enable = 1;
        data->mc_enable = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l3_rif_out_rif_part_of_lif_mngr_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_rif;
    int feature_index = dnx_data_l3_rif_out_rif_part_of_lif_mngr;
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
jr2_a0_dnx_data_l3_rif_max_nof_rifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_rif;
    int define_index = dnx_data_l3_rif_define_max_nof_rifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x20000;

    
    define->data = 0x20000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_rif_nof_rifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_rif;
    int define_index = dnx_data_l3_rif_define_nof_rifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4096;

    
    define->data = 4096;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "rif_id_max";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_l3_rif_nof_rifs_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_l3_fwd_ipmc_rif_key_participation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_ipmc_rif_key_participation;
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
jr2_a0_dnx_data_l3_fwd_host_entry_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_host_entry_support;
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
jr2_a0_dnx_data_l3_fwd_tcam_entry_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_tcam_entry_support;
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
jr2_a0_dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group;
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
jr2_a0_dnx_data_l3_fwd_lpm_default_entry_set_by_prefix_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_lpm_default_entry_set_by_prefix;
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
jr2_a0_dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support;
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
jr2_a0_dnx_data_l3_fwd_native_arp_rif_encap_access_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int feature_index = dnx_data_l3_fwd_native_arp_rif_encap_access_support;
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
jr2_a0_dnx_data_l3_fwd_flp_fragment_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_flp_fragment_support;
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
jr2_a0_dnx_data_l3_fwd_max_mc_group_lpm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_max_mc_group_lpm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1FFFF;

    
    define->data = 0x1FFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_max_mc_group_em_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_max_mc_group_em;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7FFFF;

    
    define->data = 0x7FFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_nof_my_mac_prefixes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_my_mac_prefixes;
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
jr2_a0_dnx_data_l3_fwd_compressed_sip_svl_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_compressed_sip_svl_size;
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
jr2_a0_dnx_data_l3_fwd_nof_compressed_svl_sip_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_compressed_svl_sip;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_l3.fwd.compressed_sip_svl_size_get(unit);

    
    define->data = 1 << dnx_data_l3.fwd.compressed_sip_svl_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_lpm_strength_profiles_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_lpm_strength_profiles_support;
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
jr2_a0_dnx_data_l3_fwd_nof_lpm_strength_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_lpm_strength_profiles;
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
jr2_a0_dnx_data_l3_fwd_compressed_sip_ivl_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_compressed_sip_ivl_size;
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
jr2_a0_dnx_data_l3_fwd_nof_compressed_ivl_sip_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_compressed_ivl_sip;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_l3.fwd.compressed_sip_ivl_size_get(unit);

    
    define->data = 1 << dnx_data_l3.fwd.compressed_sip_ivl_size_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_uc_supported_route_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_uc_supported_route_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_HIT_CLEAR | BCM_L3_HIT | BCM_L3_INTERNAL_ROUTE);

    
    define->data = (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_HIT_CLEAR | BCM_L3_HIT | BCM_L3_INTERNAL_ROUTE);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_uc_supported_route_flags2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_uc_supported_route_flags2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_SCALE_ROUTE);

    
    define->data = (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_SCALE_ROUTE);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_uc_supported_kbp_route_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_uc_supported_kbp_route_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_ENCAP_SPACE_OPTIMIZED | BCM_L3_SRC_DISCARD);

    
    define->data = (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_ENCAP_SPACE_OPTIMIZED | BCM_L3_SRC_DISCARD);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_uc_supported_kbp_route_flags2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY | BCM_L3_FLAGS2_NO_PAYLOAD);

    
    define->data = (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY | BCM_L3_FLAGS2_NO_PAYLOAD);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_ipmc_supported_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_ipmc_supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = BCM_IPMC_REPLACE | BCM_IPMC_IP6 | BCM_IPMC_HIT_CLEAR | BCM_IPMC_TCAM | BCM_IPMC_RAW_ENTRY | BCM_IPMC_L2 | BCM_IPMC_HIT | BCM_IPMC_DEFAULT_ENTRY;

    
    define->data = BCM_IPMC_REPLACE | BCM_IPMC_IP6 | BCM_IPMC_HIT_CLEAR | BCM_IPMC_TCAM | BCM_IPMC_RAW_ENTRY | BCM_IPMC_L2 | BCM_IPMC_HIT | BCM_IPMC_DEFAULT_ENTRY;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_ipmc_config_supported_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_ipmc_config_supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = BCM_IPMC_L2 | BCM_IPMC_REPLACE | BCM_IPMC_HIT | BCM_IPMC_HIT_CLEAR;

    
    define->data = BCM_IPMC_L2 | BCM_IPMC_REPLACE | BCM_IPMC_HIT | BCM_IPMC_HIT_CLEAR;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL | BCM_IPMC_L2 | BCM_IPMC_HIT_CLEAR;

    
    define->data = BCM_IPMC_CONFIG_TRAVERSE_DELETE_ALL | BCM_IPMC_L2 | BCM_IPMC_HIT_CLEAR;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_last_kaps_intf_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_last_kaps_intf_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3;

    
    define->data = DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_l3_fwd_lpm_profile_to_entry_strength_set(
    int unit)
{
    int lpm_profile_index;
    int kaps_intf_index;
    dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int table_index = dnx_data_l3_fwd_table_lpm_profile_to_entry_strength;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;
    table->keys[1].size = 4;
    table->info_get.key_size[1] = 4;

    
    table->values[0].default_val = "-1,-1,-1,-1,-1,-1,-1";
    table->values[1].default_val = "-1,-1,-1,-1,-1,-1,-1";
    table->values[2].default_val = "1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fwd_lpm_profile_to_entry_strength_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_l3_fwd_table_lpm_profile_to_entry_strength");

    
    default_data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    SHR_RANGE_VERIFY(7, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->prefix_len_non_def[0] = -1;
    default_data->prefix_len_non_def[1] = -1;
    default_data->prefix_len_non_def[2] = -1;
    default_data->prefix_len_non_def[3] = -1;
    default_data->prefix_len_non_def[4] = -1;
    default_data->prefix_len_non_def[5] = -1;
    default_data->prefix_len_non_def[6] = -1;
    SHR_RANGE_VERIFY(7, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->prefix_len_def[0] = -1;
    default_data->prefix_len_def[1] = -1;
    default_data->prefix_len_def[2] = -1;
    default_data->prefix_len_def[3] = -1;
    default_data->prefix_len_def[4] = -1;
    default_data->prefix_len_def[5] = -1;
    default_data->prefix_len_def[6] = -1;
    default_data->entry_strength_non_def[0] = 1;
    default_data->entry_strength_def[0] = -1;
    
    for (lpm_profile_index = 0; lpm_profile_index < table->keys[0].size; lpm_profile_index++)
    {
        for (kaps_intf_index = 0; kaps_intf_index < table->keys[1].size; kaps_intf_index++)
        {
            data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, lpm_profile_index, kaps_intf_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_0 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_0);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_1 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_1);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 51;
        data->prefix_len_non_def[2] = 63;
        data->prefix_len_non_def[3] = 64;
        data->prefix_len_non_def[4] = 96;
        data->prefix_len_non_def[5] = 144;
        data->prefix_len_non_def[6] = 160;
        data->prefix_len_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = 2;
        data->entry_strength_non_def[2] = 3;
        data->entry_strength_non_def[3] = 4;
        data->entry_strength_non_def[4] = 5;
        data->entry_strength_non_def[5] = 5;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_2 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_2);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }
    if (0 < table->keys[0].size && DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_lpm_profile_to_entry_strength_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_ENUM_FVAL_KAPS_INTERFACE_INTERFACE_3);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_L3_FWD_NOF_LPM_STRENGTH_PROFILES, _SHR_E_INTERNAL, "out of bound access to array")
        data->entry_strength_def[0] = 0;
        data->entry_strength_def[1] = -1;
        data->entry_strength_def[2] = -1;
        data->entry_strength_def[3] = -1;
        data->entry_strength_def[4] = -1;
        data->entry_strength_def[5] = -1;
        data->entry_strength_def[6] = -1;
        data->entry_strength_def[7] = -1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_routing_tables_set(
    int unit)
{
    int ip_version_index;
    dnx_data_l3_fwd_routing_tables_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int table_index = dnx_data_l3_fwd_table_routing_tables;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fwd_routing_tables_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_fwd_table_routing_tables");

    
    default_data = (dnx_data_l3_fwd_routing_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tables[0] = 0;
    
    for (ip_version_index = 0; ip_version_index < table->keys[0].size; ip_version_index++)
    {
        data = (dnx_data_l3_fwd_routing_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, ip_version_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (L3_IPV4_PROTOCOL_VERSION < table->keys[0].size)
    {
        data = (dnx_data_l3_fwd_routing_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV4_PROTOCOL_VERSION, 0);
        SHR_RANGE_VERIFY(5, 0, L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION+1, _SHR_E_INTERNAL, "out of bound access to array")
        data->tables[0] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        data->tables[1] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;
        data->tables[2] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        data->tables[3] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
        data->tables[4] = DBAL_TABLE_EMPTY;
    }
    if (L3_IPV6_PROTOCOL_VERSION < table->keys[0].size)
    {
        data = (dnx_data_l3_fwd_routing_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV6_PROTOCOL_VERSION, 0);
        SHR_RANGE_VERIFY(5, 0, L3_MAX_NOF_ROUTING_TABLES_PER_PROTOCOL_VERSION+1, _SHR_E_INTERNAL, "out of bound access to array")
        data->tables[0] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
        data->tables[1] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
        data->tables[2] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
        data->tables[3] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
        data->tables[4] = DBAL_TABLE_EMPTY;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_mc_tables_set(
    int unit)
{
    int ip_version_index;
    int route_enable_index;
    dnx_data_l3_fwd_mc_tables_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int table_index = dnx_data_l3_fwd_table_mc_tables;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fwd_mc_tables_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_l3_fwd_table_mc_tables");

    
    default_data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tables[0] = 0;
    default_data->count = 0;
    
    for (ip_version_index = 0; ip_version_index < table->keys[0].size; ip_version_index++)
    {
        for (route_enable_index = 0; route_enable_index < table->keys[1].size; route_enable_index++)
        {
            data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, ip_version_index, route_enable_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (L3_IPV4_PROTOCOL_VERSION < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV4_PROTOCOL_VERSION, 1);
        data->tables[0] = DBAL_TABLE_IPV4_MULTICAST_EM_FORWARD;
        data->tables[1] = DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD;
        data->tables[2] = DBAL_TABLE_IPV4_MULTICAST_PUBLIC_LPM_FORWARD;
        data->tables[3] = DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD;
        data->tables[4] = DBAL_TABLE_KBP_IPV4_MULTICAST_TCAM_FORWARD;
        data->count = 5;
    }
    if (L3_IPV4_PROTOCOL_VERSION < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV4_PROTOCOL_VERSION, 0);
        data->tables[0] = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_IVL;
        data->tables[1] = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SVL;
        data->tables[2] = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_SVL;
        data->tables[3] = DBAL_TABLE_BRIDGE_IPV4_MULTICAST_SOURCE_SPECIFIC_IVL;
        data->count = 4;
    }
    if (L3_IPV6_PROTOCOL_VERSION < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV6_PROTOCOL_VERSION, 1);
        data->tables[0] = DBAL_TABLE_IPV6_MULTICAST_EM_FORWARD;
        data->tables[1] = DBAL_TABLE_IPV6_MULTICAST_PRIVATE_LPM_FORWARD;
        data->tables[2] = DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD;
        data->tables[3] = DBAL_TABLE_KBP_IPV6_MULTICAST_TCAM_FORWARD;
        data->count = 4;
    }
    if (L3_IPV6_PROTOCOL_VERSION < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_mc_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_IPV6_PROTOCOL_VERSION, 0);
        data->tables[0] = DBAL_TABLE_BRIDGE_IPV6_MULTICAST_SOURCE_SPECIFIC_SVL;
        data->tables[1] = DBAL_TABLE_BRIDGE_IPV6_MULTICAST_SOURCE_SPECIFIC_IVL;
        data->count = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_fwd_ipmc_compression_mode_set(
    int unit)
{
    int route_enable_index;
    int dbal_table_type_index;
    dnx_data_l3_fwd_ipmc_compression_mode_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int table_index = dnx_data_l3_fwd_table_ipmc_compression_mode;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
    table->keys[1].size = DBAL_NOF_TABLE_TYPES;
    table->info_get.key_size[1] = DBAL_NOF_TABLE_TYPES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DBAL_NOF_TABLES";
    table->values[2].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    table->values[3].default_val = "DBAL_NOF_FIELDS";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_fwd_ipmc_compression_mode_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_l3_fwd_table_ipmc_compression_mode");

    
    default_data = (dnx_data_l3_fwd_ipmc_compression_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->mode = 0;
    default_data->dbal_table = DBAL_NOF_TABLES;
    default_data->physical_table = DBAL_NOF_PHYSICAL_TABLES;
    default_data->result_field = DBAL_NOF_FIELDS;
    
    for (route_enable_index = 0; route_enable_index < table->keys[0].size; route_enable_index++)
    {
        for (dbal_table_type_index = 0; dbal_table_type_index < table->keys[1].size; dbal_table_type_index++)
        {
            data = (dnx_data_l3_fwd_ipmc_compression_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, route_enable_index, dbal_table_type_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (1 < table->keys[0].size && DBAL_TABLE_TYPE_LPM < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_ipmc_compression_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, DBAL_TABLE_TYPE_LPM);
        data->mode = IPMC_CONFIG_CMPRS_SIP_INTF;
        data->dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE_AND_INTERFACE;
        data->physical_table = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->result_field = DBAL_FIELD_KAPS_RESULT;
    }
    if (0 < table->keys[0].size && DBAL_TABLE_TYPE_LPM < table->keys[1].size)
    {
        data = (dnx_data_l3_fwd_ipmc_compression_mode_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, DBAL_TABLE_TYPE_LPM);
        data->mode = IPMC_CONFIG_CMPRS_SIP;
        data->dbal_table = DBAL_TABLE_IPV6_MULTICAST_SOURCE;
        data->physical_table = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->result_field = DBAL_FIELD_KAPS_RESULT;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int feature_index = dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support;
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
jr2_a0_dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits;
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
jr2_a0_dnx_data_l3_ecmp_group_size_consistent_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_group_size_consistent_nof_bits;
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
jr2_a0_dnx_data_l3_ecmp_group_size_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_group_size_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit),dnx_data_l3.ecmp.group_size_consistent_nof_bits_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_l3.ecmp.group_size_multiply_and_divide_nof_bits_get(unit),dnx_data_l3.ecmp.group_size_consistent_nof_bits_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_max_group_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_max_group_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_l3.ecmp.group_size_nof_bits_get(unit);

    
    define->data = 1 << dnx_data_l3.ecmp.group_size_nof_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_basic_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp_basic_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x8000;

    
    define->data = 0x8000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_total_nof_ecmp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_total_nof_ecmp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xA000;

    
    define->data = 0xA000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_max_ecmp_basic_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_max_ecmp_basic_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit)-1;

    
    define->data = dnx_data_l3.ecmp.nof_ecmp_basic_mode_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_max_ecmp_extended_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_max_ecmp_extended_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x9FFF;

    
    define->data = 0x9FFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x200;

    
    define->data = 0x200;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.ecmp.ecmp_bank_size_get(unit) / dnx_data_l3.ecmp.nof_extended_ecmp_per_failover_bank_get(unit);

    
    define->data = dnx_data_l3.ecmp.ecmp_bank_size_get(unit) / dnx_data_l3.ecmp.nof_extended_ecmp_per_failover_bank_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x400;

    
    define->data = 0x400;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits;
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
jr2_a0_dnx_data_l3_ecmp_member_table_address_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_member_table_address_size_in_bits;
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
jr2_a0_dnx_data_l3_ecmp_ecmp_bank_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_ecmp_bank_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x800;

    
    define->data = 0x800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_banks_basic_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp_banks_basic;
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
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_banks_extended_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp_banks_extended;
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
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xA000;

    
    define->data = 0xA000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_profile_id_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_profile_id_size;
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
jr2_a0_dnx_data_l3_ecmp_profile_id_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_profile_id_offset;
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
jr2_a0_dnx_data_l3_ecmp_tunnel_priority_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_support;
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
jr2_a0_dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles;
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
jr2_a0_dnx_data_l3_ecmp_tunnel_priority_field_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_field_width;
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
jr2_a0_dnx_data_l3_ecmp_tunnel_priority_index_field_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_tunnel_priority_index_field_width;
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
jr2_a0_dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits;
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
jr2_a0_dnx_data_l3_ecmp_consistent_bank_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_consistent_bank_size_in_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x2000;

    
    define->data = 0x2000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries;
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
jr2_a0_dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits;
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
jr2_a0_dnx_data_l3_ecmp_nof_consistent_resources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_consistent_resources;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit) * (1 << dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get(unit)) / dnx_data_l3.ecmp.ecmp_small_consistent_group_entry_size_in_bits_get(unit) / dnx_data_l3.ecmp.ecmp_small_consistent_group_nof_entries_get(unit);

    
    define->data = dnx_data_l3.ecmp.consistent_mem_row_size_in_bits_get(unit) * (1 << dnx_data_l3.ecmp.member_table_nof_rows_size_in_bits_get(unit)) / dnx_data_l3.ecmp.ecmp_small_consistent_group_entry_size_in_bits_get(unit) / dnx_data_l3.ecmp.ecmp_small_consistent_group_nof_entries_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_l3_ecmp_consistent_tables_info_set(
    int unit)
{
    int table_type_index;
    dnx_data_l3_ecmp_consistent_tables_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int table_index = dnx_data_l3_ecmp_table_consistent_tables_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_ecmp_consistent_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_ecmp_table_consistent_tables_info");

    
    default_data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_entries = 0;
    default_data->entry_size_in_bits = 0;
    
    for (table_type_index = 0; table_type_index < table->keys[0].size; table_type_index++)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, table_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE, 0);
        data->nof_entries =  dnx_data_l3.ecmp.ecmp_small_consistent_group_nof_entries_get(unit);
        data->entry_size_in_bits = dnx_data_l3.ecmp.ecmp_small_consistent_group_entry_size_in_bits_get(unit);
    }
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE, 0);
        data->nof_entries = 256;
        data->entry_size_in_bits = 8;
    }
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE, 0);
        data->nof_entries = 512;
        data->entry_size_in_bits = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l3_vip_ecmp_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vip_ecmp;
    int feature_index = dnx_data_l3_vip_ecmp_supported;
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
jr2_a0_dnx_data_l3_vip_ecmp_nof_vip_ecmp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vip_ecmp;
    int define_index = dnx_data_l3_vip_ecmp_define_nof_vip_ecmp;
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
jr2_a0_dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vip_ecmp;
    int define_index = dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16384;

    
    define->data = 16384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_l3_vrrp_nof_protocol_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_nof_protocol_groups;
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
jr2_a0_dnx_data_l3_vrrp_nof_tcam_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_nof_tcam_entries;
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
jr2_a0_dnx_data_l3_vrrp_nof_vsi_tcam_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_nof_vsi_tcam_entries;
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
jr2_a0_dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem;
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
jr2_a0_dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index;
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
jr2_a0_dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index;
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
jr2_a0_dnx_data_l3_vrrp_exem_default_tcam_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_exem_default_tcam_index;
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
jr2_a0_dnx_data_l3_vrrp_exem_default_tcam_index_msb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_exem_default_tcam_index_msb;
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
jr2_a0_dnx_data_l3_vrrp_vrrp_default_num_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_vrrp_default_num_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit) - dnx_data_l3.vrrp.nof_tcam_entries_used_by_exem_get(unit);

    
    define->data = dnx_data_l3.vrrp.nof_vsi_tcam_entries_get(unit) - dnx_data_l3.vrrp.nof_tcam_entries_used_by_exem_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_vrrp_is_mapped_pp_port_used_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_is_mapped_pp_port_used;
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
jr2_a0_dnx_data_l3_vrrp_nof_dbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int define_index = dnx_data_l3_vrrp_define_nof_dbs;
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
jr2_a0_dnx_data_l3_vrrp_vrrp_tables_set(
    int unit)
{
    int table_type_index;
    dnx_data_l3_vrrp_vrrp_tables_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int table_index = dnx_data_l3_vrrp_table_vrrp_tables;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = L3_VRRP_TABLE_NOF_TABLES;
    table->info_get.key_size[0] = L3_VRRP_TABLE_NOF_TABLES;

    
    table->values[0].default_val = "DBAL_NOF_TABLES";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_vrrp_vrrp_tables_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_vrrp_table_vrrp_tables");

    
    default_data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table[0] = DBAL_NOF_TABLES;
    
    for (table_type_index = 0; table_type_index < table->keys[0].size; table_type_index++)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, table_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (L3_VRRP_TABLE_PER_PORT_MY_MAC < table->keys[0].size)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_VRRP_TABLE_PER_PORT_MY_MAC, 0);
        data->table[0] = DBAL_TABLE_EXEM_PER_PORT_MY_MAC;
        data->table[1] = DBAL_TABLE_EXEM3_PER_PORT_MY_MAC;
    }
    if (L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0 < table->keys[0].size)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0, 0);
        data->table[0] = DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0;
        data->table[1] = DBAL_TABLE_EXEM3_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_0;
    }
    if (L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1 < table->keys[0].size)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_VRRP_TABLE_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1, 0);
        data->table[0] = DBAL_TABLE_EXEM_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1;
        data->table[1] = DBAL_TABLE_EXEM3_PER_VSI_MULTIPLE_MY_MAC_DA_MSB_1;
    }
    if (L3_VRRP_TABLE_VRID_MY_MAC_WITH_PROTOCOL < table->keys[0].size)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_VRRP_TABLE_VRID_MY_MAC_WITH_PROTOCOL, 0);
        data->table[0] = DBAL_TABLE_EXEM_VRID_MY_MAC_WITH_PROTOCOL;
        data->table[1] = DBAL_TABLE_EXEM3_VRID_MY_MAC_WITH_PROTOCOL;
    }
    if (L3_VRRP_TABLE_PER_PORT_IGNORE_MAC < table->keys[0].size)
    {
        data = (dnx_data_l3_vrrp_vrrp_tables_t *) dnxc_data_mgmt_table_data_get(unit, table, L3_VRRP_TABLE_PER_PORT_IGNORE_MAC, 0);
        data->table[0] = DBAL_TABLE_EXEM_PER_PORT_IGNORE_MAC;
        data->table[1] = DBAL_TABLE_EXEM3_PER_PORT_IGNORE_MAC;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_vrrp_data_bases_set(
    int unit)
{
    dnx_data_l3_vrrp_data_bases_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_vrrp;
    int table_index = dnx_data_l3_vrrp_table_data_bases;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "DBAL_NOF_PHYSICAL_TABLES";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_vrrp_data_bases_t, (1 + 1  ), "data of dnx_data_l3_vrrp_table_data_bases");

    
    default_data = (dnx_data_l3_vrrp_data_bases_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->data_base[0] = DBAL_NOF_PHYSICAL_TABLES;
    
    data = (dnx_data_l3_vrrp_data_bases_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    data = (dnx_data_l3_vrrp_data_bases_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_L3_VRRP_NOF_DBS, _SHR_E_INTERNAL, "out of bound access to array")
    data->data_base[0] = DBAL_PHYSICAL_TABLE_SEXEM_1;
    data->data_base[1] = DBAL_PHYSICAL_TABLE_SEXEM_3;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_l3_feature_ipv6_mc_compatible_dmac_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_ipv6_mc_compatible_dmac;
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
jr2_a0_dnx_data_l3_feature_fec_hit_bit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fec_hit_bit;
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
jr2_a0_dnx_data_l3_feature_fer_fec_granularity_double_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fer_fec_granularity_double_size;
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
jr2_a0_dnx_data_l3_feature_mc_bridge_fallback_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_mc_bridge_fallback;
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
jr2_a0_dnx_data_l3_feature_nat_on_a_stick_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_nat_on_a_stick;
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
jr2_a0_dnx_data_l3_feature_routed_learning_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_routed_learning;
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
jr2_a0_dnx_data_l3_feature_ingress_frag_not_first_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_ingress_frag_not_first;
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
jr2_a0_dnx_data_l3_feature_fec_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_fec_init_enable;
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
jr2_a0_dnx_data_l3_feature_public_routing_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_public_routing_support;
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
jr2_a0_dnx_data_l3_feature_separate_fwd_rpf_dbs_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_separate_fwd_rpf_dbs;
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
jr2_a0_dnx_data_l3_feature_default_fec_limitation_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_default_fec_limitation;
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
jr2_a0_dnx_data_l3_feature_wcmp_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_wcmp_support;
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
jr2_a0_dnx_data_l3_feature_default_kaps_interface_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_default_kaps_interface;
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
jr2_a0_dnx_data_l3_feature_lpm_uses_lart_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_lpm_uses_lart;
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
jr2_a0_dnx_data_l3_feature_my_mac_prefix_0_is_invalid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_my_mac_prefix_0_is_invalid;
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
jr2_a0_dnx_data_l3_feature_vpbr_used_for_bfd_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_vpbr_used_for_bfd;
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
jr2_a0_dnx_data_l3_feature_vpbr_tunnel_available_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_vpbr_tunnel_available;
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
jr2_a0_dnx_data_l3_feature_mc_bridge_fallback_outlifs_set_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_mc_bridge_fallback_outlifs_set;
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
jr2_a0_dnx_data_l3_feature_rpf_default_route_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_rpf_default_route_supported;
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
jr2_a0_dnx_data_l3_feature_anti_spoofing_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_anti_spoofing_support;
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
jr2_a0_dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_feature;
    int feature_index = dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "l3_ipv6_mc_tcam_not_used";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_l3_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_l3;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_l3_submodule_egr_pointed;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_egr_pointed_define_first_egr_pointed_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_egr_pointed_first_egr_pointed_id_set;
    data_index = dnx_data_l3_egr_pointed_define_nof_egr_pointed_ids;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_egr_pointed_nof_egr_pointed_ids_set;
    data_index = dnx_data_l3_egr_pointed_define_nof_bits_in_egr_pointed_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_egr_pointed_nof_bits_in_egr_pointed_id_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_fec;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fec_define_nof_fecs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_fecs_set;
    data_index = dnx_data_l3_fec_define_first_valid_fec_ecmp_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_first_valid_fec_ecmp_id_set;
    data_index = dnx_data_l3_fec_define_max_fec_id_for_single_dhb_cluster_pair_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_fec_id_for_single_dhb_cluster_pair_granularity_set;
    data_index = dnx_data_l3_fec_define_max_fec_id_for_double_dhb_cluster_pair_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_fec_id_for_double_dhb_cluster_pair_granularity_set;
    data_index = dnx_data_l3_fec_define_max_default_fec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_default_fec_set;
    data_index = dnx_data_l3_fec_define_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_bank_size_set;
    data_index = dnx_data_l3_fec_define_bank_size_round_up;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_bank_size_round_up_set;
    data_index = dnx_data_l3_fec_define_max_nof_super_fecs_per_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_nof_super_fecs_per_bank_set;
    data_index = dnx_data_l3_fec_define_nof_physical_fecs_per_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_physical_fecs_per_bank_set;
    data_index = dnx_data_l3_fec_define_bank_nof_physical_rows;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_bank_nof_physical_rows_set;
    data_index = dnx_data_l3_fec_define_super_fec_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_super_fec_size_set;
    data_index = dnx_data_l3_fec_define_max_nof_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_nof_banks_set;
    data_index = dnx_data_l3_fec_define_first_bank_without_id_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_first_bank_without_id_alloc_set;
    data_index = dnx_data_l3_fec_define_first_shared_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_first_shared_bank_set;
    data_index = dnx_data_l3_fec_define_max_super_fec_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_super_fec_id_set;
    data_index = dnx_data_l3_fec_define_nof_fec_directions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_fec_directions_set;
    data_index = dnx_data_l3_fec_define_nof_dpc_fec_db;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_dpc_fec_db_set;
    data_index = dnx_data_l3_fec_define_nof_fec_dbs_all_cores;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_fec_dbs_all_cores_set;
    data_index = dnx_data_l3_fec_define_nof_fec_result_types;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_nof_fec_result_types_set;
    data_index = dnx_data_l3_fec_define_fer_hw_version;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_fer_hw_version_set;
    data_index = dnx_data_l3_fec_define_fec_property_supported_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_fec_property_supported_flags_set;
    data_index = dnx_data_l3_fec_define_supported_fec_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_supported_fec_flags_set;
    data_index = dnx_data_l3_fec_define_supported_fec_flags2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_supported_fec_flags2_set;

    
    data_index = dnx_data_l3_fec_uneven_bank_sizes;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fec_uneven_bank_sizes_set;
    data_index = dnx_data_l3_fec_mc_rpf_sip_based_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fec_mc_rpf_sip_based_supported_set;

    
    data_index = dnx_data_l3_fec_table_fec_resource_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_fec_resource_info_set;
    data_index = dnx_data_l3_fec_table_super_fec_result_types_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_super_fec_result_types_map_set;
    data_index = dnx_data_l3_fec_table_fec_result_type_fields;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_fec_result_type_fields_set;
    data_index = dnx_data_l3_fec_table_fec_physical_db;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_fec_physical_db_set;
    data_index = dnx_data_l3_fec_table_fec_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_fec_tables_info_set;
    data_index = dnx_data_l3_fec_table_fec_hierachy_position_in_range;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fec_fec_hierachy_position_in_range_set;
    
    submodule_index = dnx_data_l3_submodule_fer;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fer_define_mdb_granularity_ratio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fer_mdb_granularity_ratio_set;

    
    data_index = dnx_data_l3_fer_mux_connection_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fer_mux_connection_supported_set;

    
    
    submodule_index = dnx_data_l3_submodule_source_address;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_source_address_define_source_address_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_source_address_table_size_set;
    data_index = dnx_data_l3_source_address_define_nof_mymac_prefixes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_nof_mymac_prefixes_set;
    data_index = dnx_data_l3_source_address_define_max_nof_mac_addresses_used_by_arp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_max_nof_mac_addresses_used_by_arp_set;
    data_index = dnx_data_l3_source_address_define_custom_sa_use_dual_homing;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_custom_sa_use_dual_homing_set;

    

    
    data_index = dnx_data_l3_source_address_table_address_type_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_source_address_address_type_info_set;
    
    submodule_index = dnx_data_l3_submodule_vrf;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_vrf_define_nof_vrf;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrf_nof_vrf_set;
    data_index = dnx_data_l3_vrf_define_nof_vrf_ipv6;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrf_nof_vrf_ipv6_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_routing_enablers;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_routing_enablers_define_routing_enable_vector_length;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enable_vector_length_set;
    data_index = dnx_data_l3_routing_enablers_define_routing_enable_action_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enable_action_support_set;
    data_index = dnx_data_l3_routing_enablers_define_routing_enable_action_profile_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enable_action_profile_size_set;
    data_index = dnx_data_l3_routing_enablers_define_routing_enable_indication_nof_bits_per_layer;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enable_indication_nof_bits_per_layer_set;
    data_index = dnx_data_l3_routing_enablers_define_routing_enable_nof_profiles_per_layer;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enable_nof_profiles_per_layer_set;
    data_index = dnx_data_l3_routing_enablers_define_routing_enabled_action_profile_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_routing_enabled_action_profile_id_set;

    
    data_index = dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_routing_enablers_ptc_routing_enable_profile_support_set;

    
    data_index = dnx_data_l3_routing_enablers_table_layer_enablers_by_id;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_routing_enablers_layer_enablers_by_id_set;
    data_index = dnx_data_l3_routing_enablers_table_layer_enablers;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_routing_enablers_layer_enablers_set;
    
    submodule_index = dnx_data_l3_submodule_rif;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_rif_define_max_nof_rifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_rif_max_nof_rifs_set;
    data_index = dnx_data_l3_rif_define_nof_rifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_rif_nof_rifs_set;

    
    data_index = dnx_data_l3_rif_out_rif_part_of_lif_mngr;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_rif_out_rif_part_of_lif_mngr_set;

    
    
    submodule_index = dnx_data_l3_submodule_fwd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_fwd_define_flp_fragment_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_flp_fragment_support_set;
    data_index = dnx_data_l3_fwd_define_max_mc_group_lpm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_max_mc_group_lpm_set;
    data_index = dnx_data_l3_fwd_define_max_mc_group_em;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_max_mc_group_em_set;
    data_index = dnx_data_l3_fwd_define_nof_my_mac_prefixes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_my_mac_prefixes_set;
    data_index = dnx_data_l3_fwd_define_compressed_sip_svl_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_compressed_sip_svl_size_set;
    data_index = dnx_data_l3_fwd_define_nof_compressed_svl_sip;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_compressed_svl_sip_set;
    data_index = dnx_data_l3_fwd_define_lpm_strength_profiles_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_lpm_strength_profiles_support_set;
    data_index = dnx_data_l3_fwd_define_nof_lpm_strength_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_lpm_strength_profiles_set;
    data_index = dnx_data_l3_fwd_define_compressed_sip_ivl_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_compressed_sip_ivl_size_set;
    data_index = dnx_data_l3_fwd_define_nof_compressed_ivl_sip;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_compressed_ivl_sip_set;
    data_index = dnx_data_l3_fwd_define_uc_supported_route_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_uc_supported_route_flags_set;
    data_index = dnx_data_l3_fwd_define_uc_supported_route_flags2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_uc_supported_route_flags2_set;
    data_index = dnx_data_l3_fwd_define_uc_supported_kbp_route_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_uc_supported_kbp_route_flags_set;
    data_index = dnx_data_l3_fwd_define_uc_supported_kbp_route_flags2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_uc_supported_kbp_route_flags2_set;
    data_index = dnx_data_l3_fwd_define_ipmc_supported_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_ipmc_supported_flags_set;
    data_index = dnx_data_l3_fwd_define_ipmc_config_supported_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_ipmc_config_supported_flags_set;
    data_index = dnx_data_l3_fwd_define_ipmc_config_supported_traverse_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_ipmc_config_supported_traverse_flags_set;
    data_index = dnx_data_l3_fwd_define_last_kaps_intf_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_last_kaps_intf_id_set;

    
    data_index = dnx_data_l3_fwd_ipmc_rif_key_participation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_ipmc_rif_key_participation_set;
    data_index = dnx_data_l3_fwd_host_entry_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_host_entry_support_set;
    data_index = dnx_data_l3_fwd_tcam_entry_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_tcam_entry_support_set;
    data_index = dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_ipmc_config_cmprs_vrf_group_set;
    data_index = dnx_data_l3_fwd_lpm_default_entry_set_by_prefix;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_lpm_default_entry_set_by_prefix_set;
    data_index = dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_kaps_lpm_strength_profile_map_support_set;
    data_index = dnx_data_l3_fwd_native_arp_rif_encap_access_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_fwd_native_arp_rif_encap_access_support_set;

    
    data_index = dnx_data_l3_fwd_table_lpm_profile_to_entry_strength;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fwd_lpm_profile_to_entry_strength_set;
    data_index = dnx_data_l3_fwd_table_routing_tables;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fwd_routing_tables_set;
    data_index = dnx_data_l3_fwd_table_mc_tables;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fwd_mc_tables_set;
    data_index = dnx_data_l3_fwd_table_ipmc_compression_mode;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fwd_ipmc_compression_mode_set;
    
    submodule_index = dnx_data_l3_submodule_ecmp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_ecmp_define_group_size_multiply_and_divide_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_group_size_multiply_and_divide_nof_bits_set;
    data_index = dnx_data_l3_ecmp_define_group_size_consistent_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_group_size_consistent_nof_bits_set;
    data_index = dnx_data_l3_ecmp_define_group_size_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_group_size_nof_bits_set;
    data_index = dnx_data_l3_ecmp_define_max_group_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_max_group_size_set;
    data_index = dnx_data_l3_ecmp_define_nof_ecmp_basic_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_basic_mode_set;
    data_index = dnx_data_l3_ecmp_define_total_nof_ecmp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_total_nof_ecmp_set;
    data_index = dnx_data_l3_ecmp_define_max_ecmp_basic_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_max_ecmp_basic_mode_set;
    data_index = dnx_data_l3_ecmp_define_max_ecmp_extended_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_max_ecmp_extended_mode_set;
    data_index = dnx_data_l3_ecmp_define_nof_extended_ecmp_per_failover_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_extended_ecmp_per_failover_bank_set;
    data_index = dnx_data_l3_ecmp_define_nof_failover_banks_per_extended_ecmp_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_failover_banks_per_extended_ecmp_bank_set;
    data_index = dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_set;
    data_index = dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_member_table_address_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_member_table_address_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_ecmp_bank_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_ecmp_bank_size_set;
    data_index = dnx_data_l3_ecmp_define_nof_ecmp_banks_basic;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_banks_basic_set;
    data_index = dnx_data_l3_ecmp_define_nof_ecmp_banks_extended;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_banks_extended_set;
    data_index = dnx_data_l3_ecmp_define_nof_ecmp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_set;
    data_index = dnx_data_l3_ecmp_define_profile_id_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_profile_id_size_set;
    data_index = dnx_data_l3_ecmp_define_profile_id_offset;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_profile_id_offset_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_tunnel_priority_support_set;
    data_index = dnx_data_l3_ecmp_define_nof_tunnel_priority_map_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_tunnel_priority_map_profiles_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_field_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_tunnel_priority_field_width_set;
    data_index = dnx_data_l3_ecmp_define_tunnel_priority_index_field_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_tunnel_priority_index_field_width_set;
    data_index = dnx_data_l3_ecmp_define_consistent_mem_row_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_consistent_mem_row_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_consistent_bank_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_consistent_bank_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_ecmp_small_consistent_group_nof_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_ecmp_small_consistent_group_nof_entries_set;
    data_index = dnx_data_l3_ecmp_define_ecmp_small_consistent_group_entry_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_ecmp_small_consistent_group_entry_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_nof_consistent_resources;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_consistent_resources_set;

    
    data_index = dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_ecmp_ecmp_group_profile_multiple_result_types_support_set;

    
    data_index = dnx_data_l3_ecmp_table_consistent_tables_info;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_ecmp_consistent_tables_info_set;
    
    submodule_index = dnx_data_l3_submodule_vip_ecmp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_vip_ecmp_define_nof_vip_ecmp;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vip_ecmp_nof_vip_ecmp_set;
    data_index = dnx_data_l3_vip_ecmp_define_max_vip_ecmp_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vip_ecmp_max_vip_ecmp_table_size_set;

    
    data_index = dnx_data_l3_vip_ecmp_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_vip_ecmp_supported_set;

    
    
    submodule_index = dnx_data_l3_submodule_vrrp;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_vrrp_define_nof_protocol_groups;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_nof_protocol_groups_set;
    data_index = dnx_data_l3_vrrp_define_nof_tcam_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_nof_tcam_entries_set;
    data_index = dnx_data_l3_vrrp_define_nof_vsi_tcam_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_nof_vsi_tcam_entries_set;
    data_index = dnx_data_l3_vrrp_define_nof_tcam_entries_used_by_exem;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_nof_tcam_entries_used_by_exem_set;
    data_index = dnx_data_l3_vrrp_define_exem_vrid_ipv4_tcam_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_exem_vrid_ipv4_tcam_index_set;
    data_index = dnx_data_l3_vrrp_define_exem_vrid_ipv6_tcam_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_exem_vrid_ipv6_tcam_index_set;
    data_index = dnx_data_l3_vrrp_define_exem_default_tcam_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_exem_default_tcam_index_set;
    data_index = dnx_data_l3_vrrp_define_exem_default_tcam_index_msb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_exem_default_tcam_index_msb_set;
    data_index = dnx_data_l3_vrrp_define_vrrp_default_num_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_vrrp_default_num_entries_set;
    data_index = dnx_data_l3_vrrp_define_is_mapped_pp_port_used;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_is_mapped_pp_port_used_set;
    data_index = dnx_data_l3_vrrp_define_nof_dbs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_vrrp_nof_dbs_set;

    

    
    data_index = dnx_data_l3_vrrp_table_vrrp_tables;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_vrrp_vrrp_tables_set;
    data_index = dnx_data_l3_vrrp_table_data_bases;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_vrrp_data_bases_set;
    
    submodule_index = dnx_data_l3_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l3_feature_ipv6_mc_compatible_dmac;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_ipv6_mc_compatible_dmac_set;
    data_index = dnx_data_l3_feature_fec_hit_bit;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_fec_hit_bit_set;
    data_index = dnx_data_l3_feature_fer_fec_granularity_double_size;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_fer_fec_granularity_double_size_set;
    data_index = dnx_data_l3_feature_mc_bridge_fallback;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_mc_bridge_fallback_set;
    data_index = dnx_data_l3_feature_nat_on_a_stick;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_nat_on_a_stick_set;
    data_index = dnx_data_l3_feature_routed_learning;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_routed_learning_set;
    data_index = dnx_data_l3_feature_ingress_frag_not_first;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_ingress_frag_not_first_set;
    data_index = dnx_data_l3_feature_fec_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_fec_init_enable_set;
    data_index = dnx_data_l3_feature_public_routing_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_public_routing_support_set;
    data_index = dnx_data_l3_feature_separate_fwd_rpf_dbs;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_separate_fwd_rpf_dbs_set;
    data_index = dnx_data_l3_feature_default_fec_limitation;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_default_fec_limitation_set;
    data_index = dnx_data_l3_feature_wcmp_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_wcmp_support_set;
    data_index = dnx_data_l3_feature_default_kaps_interface;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_default_kaps_interface_set;
    data_index = dnx_data_l3_feature_lpm_uses_lart;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_lpm_uses_lart_set;
    data_index = dnx_data_l3_feature_my_mac_prefix_0_is_invalid;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_my_mac_prefix_0_is_invalid_set;
    data_index = dnx_data_l3_feature_vpbr_used_for_bfd;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_vpbr_used_for_bfd_set;
    data_index = dnx_data_l3_feature_vpbr_tunnel_available;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_vpbr_tunnel_available_set;
    data_index = dnx_data_l3_feature_mc_bridge_fallback_outlifs_set;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_mc_bridge_fallback_outlifs_set_set;
    data_index = dnx_data_l3_feature_rpf_default_route_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_rpf_default_route_supported_set;
    data_index = dnx_data_l3_feature_anti_spoofing_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_anti_spoofing_support_set;
    data_index = dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_l3_feature_l3_ipv6_mc_tcam_not_used_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

