

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <shared/utilex/utilex_integer_arithmetic.h>








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
jr2_a0_dnx_data_l3_fec_fer_mdb_granularity_ratio_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fec;
    int define_index = dnx_data_l3_fec_define_fer_mdb_granularity_ratio;
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
jr2_a0_dnx_data_l3_routing_enablers_nof_routing_enablers_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_routing_enablers;
    int define_index = dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles;
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
        "Default: 4096\n"
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
jr2_a0_dnx_data_l3_fwd_lpm_strength_profile_support_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_lpm_strength_profile_support;
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
jr2_a0_dnx_data_l3_fwd_nof_lpm_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_lpm_profiles;
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
jr2_a0_dnx_data_l3_fwd_nof_prefix_ranges_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_fwd;
    int define_index = dnx_data_l3_fwd_define_nof_prefix_ranges;
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
    default_data->prefix_len_non_def[0] = -1;
    default_data->prefix_len_non_def[1] = -1;
    default_data->prefix_len_non_def[2] = -1;
    default_data->prefix_len_non_def[3] = -1;
    default_data->prefix_len_non_def[4] = -1;
    default_data->prefix_len_non_def[5] = -1;
    default_data->prefix_len_non_def[6] = -1;
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
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
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
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 51;
        data->prefix_len_non_def[2] = 63;
        data->prefix_len_non_def[3] = 64;
        data->prefix_len_non_def[4] = 96;
        data->prefix_len_non_def[5] = 160;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = 2;
        data->entry_strength_non_def[2] = 3;
        data->entry_strength_non_def[3] = 4;
        data->entry_strength_non_def[4] = 5;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
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
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
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
        data->prefix_len_non_def[0] = 0;
        data->prefix_len_non_def[1] = 160;
        data->prefix_len_non_def[2] = -1;
        data->prefix_len_non_def[3] = -1;
        data->prefix_len_non_def[4] = -1;
        data->prefix_len_non_def[5] = -1;
        data->prefix_len_non_def[6] = -1;
        data->prefix_len_non_def[7] = -1;
        data->prefix_len_def[0] = 0;
        data->prefix_len_def[1] = 160;
        data->prefix_len_def[2] = -1;
        data->prefix_len_def[3] = -1;
        data->prefix_len_def[4] = -1;
        data->prefix_len_def[5] = -1;
        data->prefix_len_def[6] = -1;
        data->prefix_len_def[7] = -1;
        data->entry_strength_non_def[0] = 1;
        data->entry_strength_non_def[1] = -1;
        data->entry_strength_non_def[2] = -1;
        data->entry_strength_non_def[3] = -1;
        data->entry_strength_non_def[4] = -1;
        data->entry_strength_non_def[5] = -1;
        data->entry_strength_non_def[6] = -1;
        data->entry_strength_non_def[7] = -1;
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
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_per_bank_of_low_ecmps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps;
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
    
    define->default_data = 0x7FFF;

    
    define->data = 0x7FFF;

    
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
    
    define->default_data = 0x97FF;

    
    define->data = 0x97FF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_l3_ecmp_nof_ecmp_per_bank_of_extended_ecmps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps;
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
jr2_a0_dnx_data_l3_ecmp_nof_fec_banks_per_extended_ecmp_bank_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank;
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
jr2_a0_dnx_data_l3_ecmp_ecmp_stage_map_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_ecmp_stage_map_size;
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
    
    define->default_data = 0x2000/0x800;

    
    define->data = 0x2000/0x800;

    
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
jr2_a0_dnx_data_l3_ecmp_member_table_row_width_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_ecmp;
    int define_index = dnx_data_l3_ecmp_define_member_table_row_width;
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
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_l3_ecmp_consistent_tables_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_l3_ecmp_table_consistent_tables_info");

    
    default_data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_entries = 0;
    default_data->entry_size_in_bits = 0;
    default_data->max_nof_unique_members = 0;
    
    for (table_type_index = 0; table_type_index < table->keys[0].size; table_type_index++)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, table_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE, 0);
        data->nof_entries = 128;
        data->entry_size_in_bits = 4;
        data->max_nof_unique_members = 16;
    }
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_MEDIUM_SIZE, 0);
        data->nof_entries = 256;
        data->entry_size_in_bits = 8;
        data->max_nof_unique_members = 256;
    }
    if (DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE < table->keys[0].size)
    {
        data = (dnx_data_l3_ecmp_consistent_tables_info_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE, 0);
        data->nof_entries = 512;
        data->entry_size_in_bits = 8;
        data->max_nof_unique_members = 256;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


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
jr2_a0_dnx_data_l3_source_address_ethernet_nof_mymac_prefixes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l3;
    int submodule_index = dnx_data_l3_submodule_source_address_ethernet;
    int define_index = dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes;
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
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


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
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
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
    data_index = dnx_data_l3_fec_define_super_fec_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_super_fec_size_set;
    data_index = dnx_data_l3_fec_define_max_nof_banks;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_nof_banks_set;
    data_index = dnx_data_l3_fec_define_first_bank_without_id_alloc;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_first_bank_without_id_alloc_set;
    data_index = dnx_data_l3_fec_define_max_super_fec_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_max_super_fec_id_set;
    data_index = dnx_data_l3_fec_define_fer_mdb_granularity_ratio;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fec_fer_mdb_granularity_ratio_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_source_address;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_source_address_define_source_address_table_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_source_address_table_size_set;
    data_index = dnx_data_l3_source_address_define_custom_sa_use_dual_homing;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_custom_sa_use_dual_homing_set;

    

    
    
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

    
    data_index = dnx_data_l3_routing_enablers_define_nof_routing_enablers_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_routing_enablers_nof_routing_enablers_profiles_set;

    

    
    
    submodule_index = dnx_data_l3_submodule_rif;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_rif_define_max_nof_rifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_rif_max_nof_rifs_set;
    data_index = dnx_data_l3_rif_define_nof_rifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_rif_nof_rifs_set;

    

    
    
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
    data_index = dnx_data_l3_fwd_define_lpm_strength_profile_support;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_lpm_strength_profile_support_set;
    data_index = dnx_data_l3_fwd_define_nof_lpm_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_lpm_profiles_set;
    data_index = dnx_data_l3_fwd_define_nof_prefix_ranges;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_fwd_nof_prefix_ranges_set;

    

    
    data_index = dnx_data_l3_fwd_table_lpm_profile_to_entry_strength;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_l3_fwd_lpm_profile_to_entry_strength_set;
    
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
    data_index = dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_low_ecmps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_per_bank_of_low_ecmps_set;
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
    data_index = dnx_data_l3_ecmp_define_nof_ecmp_per_bank_of_extended_ecmps;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_ecmp_per_bank_of_extended_ecmps_set;
    data_index = dnx_data_l3_ecmp_define_nof_fec_banks_per_extended_ecmp_bank;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_fec_banks_per_extended_ecmp_bank_set;
    data_index = dnx_data_l3_ecmp_define_nof_group_profiles_per_hierarchy;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_nof_group_profiles_per_hierarchy_set;
    data_index = dnx_data_l3_ecmp_define_member_table_nof_rows_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_member_table_nof_rows_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_member_table_address_size_in_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_member_table_address_size_in_bits_set;
    data_index = dnx_data_l3_ecmp_define_ecmp_stage_map_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_ecmp_stage_map_size_set;
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
    data_index = dnx_data_l3_ecmp_define_member_table_row_width;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_ecmp_member_table_row_width_set;
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

    

    
    
    submodule_index = dnx_data_l3_submodule_source_address_ethernet;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l3_source_address_ethernet_define_nof_mymac_prefixes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_l3_source_address_ethernet_nof_mymac_prefixes_set;

    

    
    
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

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

