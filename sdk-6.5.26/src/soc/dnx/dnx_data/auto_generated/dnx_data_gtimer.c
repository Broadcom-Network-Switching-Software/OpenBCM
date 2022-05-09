
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COUNTER

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_gtimer.h>




extern shr_error_e jr2_a0_data_gtimer_attach(
    int unit);


extern shr_error_e j2c_a0_data_gtimer_attach(
    int unit);


extern shr_error_e q2a_a0_data_gtimer_attach(
    int unit);


extern shr_error_e j2p_a0_data_gtimer_attach(
    int unit);


extern shr_error_e j2x_a0_data_gtimer_attach(
    int unit);




static shr_error_e
dnx_data_gtimer_rtp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rtp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_rtp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer rtp features");

    submodule_data->features[dnx_data_gtimer_rtp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_rtp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_rtp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_rtp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer rtp defines");

    
    submodule_data->nof_tables = _dnx_data_gtimer_rtp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer rtp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_rtp_feature_get(
    int unit,
    dnx_data_gtimer_rtp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_rtp, feature);
}











static shr_error_e
dnx_data_gtimer_fdt_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fdt";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_fdt_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer fdt features");

    submodule_data->features[dnx_data_gtimer_fdt_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_fdt_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_fdt_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_fdt_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer fdt defines");

    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fdt_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_fdt_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer fdt tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_fdt_feature_get(
    int unit,
    dnx_data_gtimer_fdt_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdt, feature);
}


uint32
dnx_data_gtimer_fdt_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdt, dnx_data_gtimer_fdt_define_block_index_size);
}

uint32
dnx_data_gtimer_fdt_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdt, dnx_data_gtimer_fdt_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_fdtl_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fdtl";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_fdtl_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer fdtl features");

    submodule_data->features[dnx_data_gtimer_fdtl_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_fdtl_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_fdtl_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_fdtl_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer fdtl defines");

    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fdtl_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_fdtl_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer fdtl tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_fdtl_feature_get(
    int unit,
    dnx_data_gtimer_fdtl_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdtl, feature);
}


uint32
dnx_data_gtimer_fdtl_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdtl, dnx_data_gtimer_fdtl_define_block_index_size);
}

uint32
dnx_data_gtimer_fdtl_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fdtl, dnx_data_gtimer_fdtl_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_pem_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pem";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_pem_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer pem features");

    submodule_data->features[dnx_data_gtimer_pem_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_pem_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_pem_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_pem_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer pem defines");

    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pem_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_pem_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer pem tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_pem_feature_get(
    int unit,
    dnx_data_gtimer_pem_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pem, feature);
}


uint32
dnx_data_gtimer_pem_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pem, dnx_data_gtimer_pem_define_block_index_size);
}

uint32
dnx_data_gtimer_pem_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pem, dnx_data_gtimer_pem_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_dhc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dhc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_dhc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer dhc features");

    submodule_data->features[dnx_data_gtimer_dhc_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_dhc_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_dhc_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_dhc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer dhc defines");

    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dhc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_dhc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer dhc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_dhc_feature_get(
    int unit,
    dnx_data_gtimer_dhc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dhc, feature);
}


uint32
dnx_data_gtimer_dhc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dhc, dnx_data_gtimer_dhc_define_block_index_size);
}

uint32
dnx_data_gtimer_dhc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dhc, dnx_data_gtimer_dhc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cdu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cdu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cdu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cdu features");

    submodule_data->features[dnx_data_gtimer_cdu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cdu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cdu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cdu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cdu defines");

    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdu_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cdu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cdu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cdu_feature_get(
    int unit,
    dnx_data_gtimer_cdu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdu, feature);
}


uint32
dnx_data_gtimer_cdu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdu, dnx_data_gtimer_cdu_define_block_index_size);
}

uint32
dnx_data_gtimer_cdu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdu, dnx_data_gtimer_cdu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_esb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "esb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_esb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer esb features");

    submodule_data->features[dnx_data_gtimer_esb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_esb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_esb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_esb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer esb defines");

    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_esb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_esb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer esb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_esb_feature_get(
    int unit,
    dnx_data_gtimer_esb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_esb, feature);
}


uint32
dnx_data_gtimer_esb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_esb, dnx_data_gtimer_esb_define_block_index_size);
}

uint32
dnx_data_gtimer_esb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_esb, dnx_data_gtimer_esb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cdum_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cdum";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cdum_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cdum features");

    submodule_data->features[dnx_data_gtimer_cdum_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cdum_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cdum_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cdum_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cdum defines");

    
    submodule_data->nof_tables = _dnx_data_gtimer_cdum_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cdum tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cdum_feature_get(
    int unit,
    dnx_data_gtimer_cdum_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdum, feature);
}











static shr_error_e
dnx_data_gtimer_mesh_topology_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mesh_topology";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_mesh_topology_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer mesh_topology features");

    submodule_data->features[dnx_data_gtimer_mesh_topology_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_mesh_topology_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_mesh_topology_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_mesh_topology_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer mesh_topology defines");

    
    submodule_data->nof_tables = _dnx_data_gtimer_mesh_topology_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer mesh_topology tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_mesh_topology_feature_get(
    int unit,
    dnx_data_gtimer_mesh_topology_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mesh_topology, feature);
}











static shr_error_e
dnx_data_gtimer_bdm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "bdm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_bdm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer bdm features");

    submodule_data->features[dnx_data_gtimer_bdm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_bdm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_bdm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_bdm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer bdm defines");

    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_min_value].name = "block_index_min_value";
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_min_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_min_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_bdm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_bdm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer bdm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_bdm_feature_get(
    int unit,
    dnx_data_gtimer_bdm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_bdm, feature);
}


uint32
dnx_data_gtimer_bdm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_bdm, dnx_data_gtimer_bdm_define_block_index_size);
}

uint32
dnx_data_gtimer_bdm_block_index_min_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_bdm, dnx_data_gtimer_bdm_define_block_index_min_value);
}

uint32
dnx_data_gtimer_bdm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_bdm, dnx_data_gtimer_bdm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_hbc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "hbc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_hbc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer hbc features");

    
    submodule_data->nof_defines = _dnx_data_gtimer_hbc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer hbc defines");

    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_hbc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_hbc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer hbc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_hbc_feature_get(
    int unit,
    dnx_data_gtimer_hbc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_hbc, feature);
}


uint32
dnx_data_gtimer_hbc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_hbc, dnx_data_gtimer_hbc_define_block_index_size);
}

uint32
dnx_data_gtimer_hbc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_hbc, dnx_data_gtimer_hbc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ecgm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ecgm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ecgm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ecgm features");

    submodule_data->features[dnx_data_gtimer_ecgm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ecgm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ecgm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ecgm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ecgm defines");

    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ecgm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ecgm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ecgm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ecgm_feature_get(
    int unit,
    dnx_data_gtimer_ecgm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ecgm, feature);
}


uint32
dnx_data_gtimer_ecgm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ecgm, dnx_data_gtimer_ecgm_define_block_index_size);
}

uint32
dnx_data_gtimer_ecgm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ecgm, dnx_data_gtimer_ecgm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cfc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cfc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cfc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cfc features");

    submodule_data->features[dnx_data_gtimer_cfc_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cfc_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cfc_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cfc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cfc defines");

    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cfc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cfc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cfc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cfc_feature_get(
    int unit,
    dnx_data_gtimer_cfc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cfc, feature);
}


uint32
dnx_data_gtimer_cfc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cfc, dnx_data_gtimer_cfc_define_block_index_size);
}

uint32
dnx_data_gtimer_cfc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cfc, dnx_data_gtimer_cfc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cgm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cgm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cgm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cgm features");

    submodule_data->features[dnx_data_gtimer_cgm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cgm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cgm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cgm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cgm defines");

    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cgm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cgm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cgm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cgm_feature_get(
    int unit,
    dnx_data_gtimer_cgm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cgm, feature);
}


uint32
dnx_data_gtimer_cgm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cgm, dnx_data_gtimer_cgm_define_block_index_size);
}

uint32
dnx_data_gtimer_cgm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cgm, dnx_data_gtimer_cgm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_crps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "crps";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_crps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer crps features");

    submodule_data->features[dnx_data_gtimer_crps_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_crps_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_crps_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_crps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer crps defines");

    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_crps_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_crps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer crps tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_crps_feature_get(
    int unit,
    dnx_data_gtimer_crps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_crps, feature);
}


uint32
dnx_data_gtimer_crps_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_crps, dnx_data_gtimer_crps_define_block_index_size);
}

uint32
dnx_data_gtimer_crps_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_crps, dnx_data_gtimer_crps_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ddha_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ddha";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ddha_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ddha features");

    submodule_data->features[dnx_data_gtimer_ddha_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ddha_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ddha_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ddha_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ddha defines");

    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ddha_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ddha_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ddha tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ddha_feature_get(
    int unit,
    dnx_data_gtimer_ddha_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddha, feature);
}


uint32
dnx_data_gtimer_ddha_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddha, dnx_data_gtimer_ddha_define_block_index_size);
}

uint32
dnx_data_gtimer_ddha_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddha, dnx_data_gtimer_ddha_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ddp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ddp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ddp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ddp features");

    submodule_data->features[dnx_data_gtimer_ddp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ddp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ddp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ddp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ddp defines");

    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ddp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ddp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ddp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ddp_feature_get(
    int unit,
    dnx_data_gtimer_ddp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddp, feature);
}


uint32
dnx_data_gtimer_ddp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddp, dnx_data_gtimer_ddp_define_block_index_size);
}

uint32
dnx_data_gtimer_ddp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ddp, dnx_data_gtimer_ddp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_dqm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dqm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_dqm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer dqm features");

    submodule_data->features[dnx_data_gtimer_dqm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_dqm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_dqm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_dqm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer dqm defines");

    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dqm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_dqm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer dqm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_dqm_feature_get(
    int unit,
    dnx_data_gtimer_dqm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dqm, feature);
}


uint32
dnx_data_gtimer_dqm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dqm, dnx_data_gtimer_dqm_define_block_index_size);
}

uint32
dnx_data_gtimer_dqm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dqm, dnx_data_gtimer_dqm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_edb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "edb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_edb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer edb features");

    submodule_data->features[dnx_data_gtimer_edb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_edb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_edb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_edb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer edb defines");

    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_edb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_edb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer edb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_edb_feature_get(
    int unit,
    dnx_data_gtimer_edb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_edb, feature);
}


uint32
dnx_data_gtimer_edb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_edb, dnx_data_gtimer_edb_define_block_index_size);
}

uint32
dnx_data_gtimer_edb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_edb, dnx_data_gtimer_edb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_epni_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "epni";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_epni_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer epni features");

    submodule_data->features[dnx_data_gtimer_epni_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_epni_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_epni_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_epni_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer epni defines");

    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_epni_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_epni_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer epni tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_epni_feature_get(
    int unit,
    dnx_data_gtimer_epni_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epni, feature);
}


uint32
dnx_data_gtimer_epni_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epni, dnx_data_gtimer_epni_define_block_index_size);
}

uint32
dnx_data_gtimer_epni_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epni, dnx_data_gtimer_epni_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_epre_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "epre";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_epre_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer epre features");

    submodule_data->features[dnx_data_gtimer_epre_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_epre_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_epre_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_epre_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer epre defines");

    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_epre_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_epre_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer epre tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_epre_feature_get(
    int unit,
    dnx_data_gtimer_epre_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epre, feature);
}


uint32
dnx_data_gtimer_epre_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epre, dnx_data_gtimer_epre_define_block_index_size);
}

uint32
dnx_data_gtimer_epre_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_epre, dnx_data_gtimer_epre_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_eps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eps";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_eps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer eps features");

    submodule_data->features[dnx_data_gtimer_eps_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_eps_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_eps_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_eps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer eps defines");

    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_eps_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_eps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer eps tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_eps_feature_get(
    int unit,
    dnx_data_gtimer_eps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_eps, feature);
}


uint32
dnx_data_gtimer_eps_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_eps, dnx_data_gtimer_eps_define_block_index_size);
}

uint32
dnx_data_gtimer_eps_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_eps, dnx_data_gtimer_eps_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_erpp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "erpp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_erpp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer erpp features");

    submodule_data->features[dnx_data_gtimer_erpp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_erpp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_erpp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_erpp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer erpp defines");

    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_erpp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_erpp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer erpp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_erpp_feature_get(
    int unit,
    dnx_data_gtimer_erpp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_erpp, feature);
}


uint32
dnx_data_gtimer_erpp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_erpp, dnx_data_gtimer_erpp_define_block_index_size);
}

uint32
dnx_data_gtimer_erpp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_erpp, dnx_data_gtimer_erpp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_etppa_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "etppa";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_etppa_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer etppa features");

    submodule_data->features[dnx_data_gtimer_etppa_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_etppa_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_etppa_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_etppa_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer etppa defines");

    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppa_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_etppa_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer etppa tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_etppa_feature_get(
    int unit,
    dnx_data_gtimer_etppa_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppa, feature);
}


uint32
dnx_data_gtimer_etppa_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppa, dnx_data_gtimer_etppa_define_block_index_size);
}

uint32
dnx_data_gtimer_etppa_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppa, dnx_data_gtimer_etppa_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_etppb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "etppb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_etppb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer etppb features");

    submodule_data->features[dnx_data_gtimer_etppb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_etppb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_etppb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_etppb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer etppb defines");

    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_etppb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer etppb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_etppb_feature_get(
    int unit,
    dnx_data_gtimer_etppb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppb, feature);
}


uint32
dnx_data_gtimer_etppb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppb, dnx_data_gtimer_etppb_define_block_index_size);
}

uint32
dnx_data_gtimer_etppb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppb, dnx_data_gtimer_etppb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_etppc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "etppc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_etppc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer etppc features");

    submodule_data->features[dnx_data_gtimer_etppc_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_etppc_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_etppc_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_etppc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer etppc defines");

    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_etppc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_etppc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer etppc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_etppc_feature_get(
    int unit,
    dnx_data_gtimer_etppc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppc, feature);
}


uint32
dnx_data_gtimer_etppc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppc, dnx_data_gtimer_etppc_define_block_index_size);
}

uint32
dnx_data_gtimer_etppc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_etppc, dnx_data_gtimer_etppc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_fqp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fqp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_fqp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer fqp features");

    submodule_data->features[dnx_data_gtimer_fqp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_fqp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_fqp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_fqp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer fqp defines");

    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_fqp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_fqp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer fqp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_fqp_feature_get(
    int unit,
    dnx_data_gtimer_fqp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fqp, feature);
}


uint32
dnx_data_gtimer_fqp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fqp, dnx_data_gtimer_fqp_define_block_index_size);
}

uint32
dnx_data_gtimer_fqp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_fqp, dnx_data_gtimer_fqp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ile_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ile";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ile_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ile features");

    submodule_data->features[dnx_data_gtimer_ile_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ile_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ile_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ile_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ile defines");

    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ile_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ile_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ile tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ile_feature_get(
    int unit,
    dnx_data_gtimer_ile_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ile, feature);
}


uint32
dnx_data_gtimer_ile_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ile, dnx_data_gtimer_ile_define_block_index_size);
}

uint32
dnx_data_gtimer_ile_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ile, dnx_data_gtimer_ile_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippa_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippa";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippa_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippa features");

    submodule_data->features[dnx_data_gtimer_ippa_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippa_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippa_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippa_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippa defines");

    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippa_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippa_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippa tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippa_feature_get(
    int unit,
    dnx_data_gtimer_ippa_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippa, feature);
}


uint32
dnx_data_gtimer_ippa_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippa, dnx_data_gtimer_ippa_define_block_index_size);
}

uint32
dnx_data_gtimer_ippa_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippa, dnx_data_gtimer_ippa_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippb features");

    submodule_data->features[dnx_data_gtimer_ippb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippb defines");

    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippb_feature_get(
    int unit,
    dnx_data_gtimer_ippb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippb, feature);
}


uint32
dnx_data_gtimer_ippb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippb, dnx_data_gtimer_ippb_define_block_index_size);
}

uint32
dnx_data_gtimer_ippb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippb, dnx_data_gtimer_ippb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippc features");

    submodule_data->features[dnx_data_gtimer_ippc_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippc_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippc_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippc defines");

    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippc_feature_get(
    int unit,
    dnx_data_gtimer_ippc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippc, feature);
}


uint32
dnx_data_gtimer_ippc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippc, dnx_data_gtimer_ippc_define_block_index_size);
}

uint32
dnx_data_gtimer_ippc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippc, dnx_data_gtimer_ippc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippd features");

    submodule_data->features[dnx_data_gtimer_ippd_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippd_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippd_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippd defines");

    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippd_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippd tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippd_feature_get(
    int unit,
    dnx_data_gtimer_ippd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippd, feature);
}


uint32
dnx_data_gtimer_ippd_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippd, dnx_data_gtimer_ippd_define_block_index_size);
}

uint32
dnx_data_gtimer_ippd_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippd, dnx_data_gtimer_ippd_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippe_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippe";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippe_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippe features");

    submodule_data->features[dnx_data_gtimer_ippe_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippe_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippe_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippe_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippe defines");

    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippe_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippe_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippe tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippe_feature_get(
    int unit,
    dnx_data_gtimer_ippe_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippe, feature);
}


uint32
dnx_data_gtimer_ippe_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippe, dnx_data_gtimer_ippe_define_block_index_size);
}

uint32
dnx_data_gtimer_ippe_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippe, dnx_data_gtimer_ippe_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ippf_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ippf";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ippf_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ippf features");

    submodule_data->features[dnx_data_gtimer_ippf_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ippf_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ippf_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ippf_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ippf defines");

    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ippf_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ippf_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ippf tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ippf_feature_get(
    int unit,
    dnx_data_gtimer_ippf_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippf, feature);
}


uint32
dnx_data_gtimer_ippf_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippf, dnx_data_gtimer_ippf_define_block_index_size);
}

uint32
dnx_data_gtimer_ippf_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ippf, dnx_data_gtimer_ippf_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ips_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ips";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ips_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ips features");

    submodule_data->features[dnx_data_gtimer_ips_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ips_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ips_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ips_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ips defines");

    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ips_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ips_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ips tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ips_feature_get(
    int unit,
    dnx_data_gtimer_ips_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ips, feature);
}


uint32
dnx_data_gtimer_ips_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ips, dnx_data_gtimer_ips_define_block_index_size);
}

uint32
dnx_data_gtimer_ips_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ips, dnx_data_gtimer_ips_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ipt_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipt";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ipt_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ipt features");

    submodule_data->features[dnx_data_gtimer_ipt_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ipt_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ipt_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ipt_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ipt defines");

    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ipt_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ipt_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ipt tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ipt_feature_get(
    int unit,
    dnx_data_gtimer_ipt_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipt, feature);
}


uint32
dnx_data_gtimer_ipt_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipt, dnx_data_gtimer_ipt_define_block_index_size);
}

uint32
dnx_data_gtimer_ipt_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipt, dnx_data_gtimer_ipt_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_iqm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "iqm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_iqm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer iqm features");

    submodule_data->features[dnx_data_gtimer_iqm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_iqm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_iqm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_iqm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer iqm defines");

    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_iqm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_iqm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer iqm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_iqm_feature_get(
    int unit,
    dnx_data_gtimer_iqm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_iqm, feature);
}


uint32
dnx_data_gtimer_iqm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_iqm, dnx_data_gtimer_iqm_define_block_index_size);
}

uint32
dnx_data_gtimer_iqm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_iqm, dnx_data_gtimer_iqm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ire_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ire";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ire_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ire features");

    submodule_data->features[dnx_data_gtimer_ire_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ire_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ire_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ire_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ire defines");

    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ire_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ire_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ire tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ire_feature_get(
    int unit,
    dnx_data_gtimer_ire_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ire, feature);
}


uint32
dnx_data_gtimer_ire_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ire, dnx_data_gtimer_ire_define_block_index_size);
}

uint32
dnx_data_gtimer_ire_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ire, dnx_data_gtimer_ire_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_itpp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "itpp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_itpp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer itpp features");

    submodule_data->features[dnx_data_gtimer_itpp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_itpp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_itpp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_itpp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer itpp defines");

    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_itpp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_itpp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer itpp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_itpp_feature_get(
    int unit,
    dnx_data_gtimer_itpp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itpp, feature);
}


uint32
dnx_data_gtimer_itpp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itpp, dnx_data_gtimer_itpp_define_block_index_size);
}

uint32
dnx_data_gtimer_itpp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itpp, dnx_data_gtimer_itpp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_itppd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "itppd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_itppd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer itppd features");

    submodule_data->features[dnx_data_gtimer_itppd_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_itppd_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_itppd_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_itppd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer itppd defines");

    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_itppd_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_itppd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer itppd tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_itppd_feature_get(
    int unit,
    dnx_data_gtimer_itppd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itppd, feature);
}


uint32
dnx_data_gtimer_itppd_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itppd, dnx_data_gtimer_itppd_define_block_index_size);
}

uint32
dnx_data_gtimer_itppd_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_itppd, dnx_data_gtimer_itppd_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_kaps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "kaps";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_kaps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer kaps features");

    submodule_data->features[dnx_data_gtimer_kaps_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_kaps_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_kaps_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_kaps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer kaps defines");

    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_kaps_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_kaps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer kaps tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_kaps_feature_get(
    int unit,
    dnx_data_gtimer_kaps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_kaps, feature);
}


uint32
dnx_data_gtimer_kaps_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_kaps, dnx_data_gtimer_kaps_define_block_index_size);
}

uint32
dnx_data_gtimer_kaps_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_kaps, dnx_data_gtimer_kaps_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_mcp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mcp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_mcp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer mcp features");

    submodule_data->features[dnx_data_gtimer_mcp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_mcp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_mcp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_mcp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer mcp defines");

    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mcp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_mcp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer mcp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_mcp_feature_get(
    int unit,
    dnx_data_gtimer_mcp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mcp, feature);
}


uint32
dnx_data_gtimer_mcp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mcp, dnx_data_gtimer_mcp_define_block_index_size);
}

uint32
dnx_data_gtimer_mcp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mcp, dnx_data_gtimer_mcp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_mrps_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mrps";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_mrps_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer mrps features");

    submodule_data->features[dnx_data_gtimer_mrps_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_mrps_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_mrps_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_mrps_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer mrps defines");

    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mrps_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_mrps_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer mrps tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_mrps_feature_get(
    int unit,
    dnx_data_gtimer_mrps_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mrps, feature);
}


uint32
dnx_data_gtimer_mrps_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mrps, dnx_data_gtimer_mrps_define_block_index_size);
}

uint32
dnx_data_gtimer_mrps_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mrps, dnx_data_gtimer_mrps_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_nmg_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "nmg";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_nmg_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer nmg features");

    submodule_data->features[dnx_data_gtimer_nmg_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_nmg_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_nmg_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_nmg_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer nmg defines");

    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_nmg_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_nmg_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer nmg tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_nmg_feature_get(
    int unit,
    dnx_data_gtimer_nmg_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nmg, feature);
}


uint32
dnx_data_gtimer_nmg_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nmg, dnx_data_gtimer_nmg_define_block_index_size);
}

uint32
dnx_data_gtimer_nmg_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nmg, dnx_data_gtimer_nmg_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ocb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ocb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ocb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ocb features");

    submodule_data->features[dnx_data_gtimer_ocb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ocb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ocb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ocb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ocb defines");

    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ocb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ocb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ocb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ocb_feature_get(
    int unit,
    dnx_data_gtimer_ocb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocb, feature);
}


uint32
dnx_data_gtimer_ocb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocb, dnx_data_gtimer_ocb_define_block_index_size);
}

uint32
dnx_data_gtimer_ocb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocb, dnx_data_gtimer_ocb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_pdm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pdm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_pdm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer pdm features");

    submodule_data->features[dnx_data_gtimer_pdm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_pdm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_pdm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_pdm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer pdm defines");

    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pdm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_pdm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer pdm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_pdm_feature_get(
    int unit,
    dnx_data_gtimer_pdm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pdm, feature);
}


uint32
dnx_data_gtimer_pdm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pdm, dnx_data_gtimer_pdm_define_block_index_size);
}

uint32
dnx_data_gtimer_pdm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pdm, dnx_data_gtimer_pdm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_pqp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pqp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_pqp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer pqp features");

    submodule_data->features[dnx_data_gtimer_pqp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_pqp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_pqp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_pqp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer pqp defines");

    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pqp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_pqp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer pqp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_pqp_feature_get(
    int unit,
    dnx_data_gtimer_pqp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pqp, feature);
}


uint32
dnx_data_gtimer_pqp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pqp, dnx_data_gtimer_pqp_define_block_index_size);
}

uint32
dnx_data_gtimer_pqp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pqp, dnx_data_gtimer_pqp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_rqp_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "rqp";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_rqp_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer rqp features");

    submodule_data->features[dnx_data_gtimer_rqp_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_rqp_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_rqp_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_rqp_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer rqp defines");

    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_rqp_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_rqp_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer rqp tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_rqp_feature_get(
    int unit,
    dnx_data_gtimer_rqp_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_rqp, feature);
}


uint32
dnx_data_gtimer_rqp_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_rqp, dnx_data_gtimer_rqp_define_block_index_size);
}

uint32
dnx_data_gtimer_rqp_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_rqp, dnx_data_gtimer_rqp_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_sch_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sch";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_sch_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer sch features");

    submodule_data->features[dnx_data_gtimer_sch_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_sch_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_sch_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_sch_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer sch defines");

    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sch_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_sch_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer sch tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_sch_feature_get(
    int unit,
    dnx_data_gtimer_sch_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sch, feature);
}


uint32
dnx_data_gtimer_sch_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sch, dnx_data_gtimer_sch_define_block_index_size);
}

uint32
dnx_data_gtimer_sch_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sch, dnx_data_gtimer_sch_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_sif_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sif";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_sif_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer sif features");

    submodule_data->features[dnx_data_gtimer_sif_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_sif_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_sif_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_sif_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer sif defines");

    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sif_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_sif_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer sif tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_sif_feature_get(
    int unit,
    dnx_data_gtimer_sif_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sif, feature);
}


uint32
dnx_data_gtimer_sif_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sif, dnx_data_gtimer_sif_define_block_index_size);
}

uint32
dnx_data_gtimer_sif_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sif, dnx_data_gtimer_sif_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_spb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "spb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_spb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer spb features");

    submodule_data->features[dnx_data_gtimer_spb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_spb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_spb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_spb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer spb defines");

    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_spb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_spb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer spb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_spb_feature_get(
    int unit,
    dnx_data_gtimer_spb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_spb, feature);
}


uint32
dnx_data_gtimer_spb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_spb, dnx_data_gtimer_spb_define_block_index_size);
}

uint32
dnx_data_gtimer_spb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_spb, dnx_data_gtimer_spb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_sqm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sqm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_sqm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer sqm features");

    submodule_data->features[dnx_data_gtimer_sqm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_sqm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_sqm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_sqm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer sqm defines");

    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_sqm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_sqm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer sqm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_sqm_feature_get(
    int unit,
    dnx_data_gtimer_sqm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sqm, feature);
}


uint32
dnx_data_gtimer_sqm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sqm, dnx_data_gtimer_sqm_define_block_index_size);
}

uint32
dnx_data_gtimer_sqm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_sqm, dnx_data_gtimer_sqm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_tcam_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tcam";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_tcam_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer tcam features");

    submodule_data->features[dnx_data_gtimer_tcam_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_tcam_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_tcam_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_tcam_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer tcam defines");

    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_tcam_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_tcam_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer tcam tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_tcam_feature_get(
    int unit,
    dnx_data_gtimer_tcam_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tcam, feature);
}


uint32
dnx_data_gtimer_tcam_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tcam, dnx_data_gtimer_tcam_define_block_index_size);
}

uint32
dnx_data_gtimer_tcam_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tcam, dnx_data_gtimer_tcam_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_tdu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tdu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_tdu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer tdu features");

    submodule_data->features[dnx_data_gtimer_tdu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_tdu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_tdu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_tdu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer tdu defines");

    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_size].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_tdu_define_block_index_max_value].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_gtimer_tdu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer tdu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_tdu_feature_get(
    int unit,
    dnx_data_gtimer_tdu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tdu, feature);
}


uint32
dnx_data_gtimer_tdu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tdu, dnx_data_gtimer_tdu_define_block_index_size);
}

uint32
dnx_data_gtimer_tdu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_tdu, dnx_data_gtimer_tdu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_dcc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dcc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_dcc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer dcc features");

    
    submodule_data->nof_defines = _dnx_data_gtimer_dcc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer dcc defines");

    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_dcc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_dcc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer dcc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_dcc_feature_get(
    int unit,
    dnx_data_gtimer_dcc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dcc, feature);
}


uint32
dnx_data_gtimer_dcc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dcc, dnx_data_gtimer_dcc_define_block_index_size);
}

uint32
dnx_data_gtimer_dcc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_dcc, dnx_data_gtimer_dcc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cdb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cdb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cdb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cdb features");

    submodule_data->features[dnx_data_gtimer_cdb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cdb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cdb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cdb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cdb defines");

    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cdb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cdb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cdb_feature_get(
    int unit,
    dnx_data_gtimer_cdb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdb, feature);
}


uint32
dnx_data_gtimer_cdb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdb, dnx_data_gtimer_cdb_define_block_index_size);
}

uint32
dnx_data_gtimer_cdb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdb, dnx_data_gtimer_cdb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cdbm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cdbm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cdbm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cdbm features");

    submodule_data->features[dnx_data_gtimer_cdbm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cdbm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cdbm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cdbm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cdbm defines");

    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdbm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cdbm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cdbm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cdbm_feature_get(
    int unit,
    dnx_data_gtimer_cdbm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdbm, feature);
}


uint32
dnx_data_gtimer_cdbm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdbm, dnx_data_gtimer_cdbm_define_block_index_size);
}

uint32
dnx_data_gtimer_cdbm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdbm, dnx_data_gtimer_cdbm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_cdpm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "cdpm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_cdpm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer cdpm features");

    submodule_data->features[dnx_data_gtimer_cdpm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_cdpm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_cdpm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_cdpm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer cdpm defines");

    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_cdpm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_cdpm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer cdpm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_cdpm_feature_get(
    int unit,
    dnx_data_gtimer_cdpm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdpm, feature);
}


uint32
dnx_data_gtimer_cdpm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdpm, dnx_data_gtimer_cdpm_define_block_index_size);
}

uint32
dnx_data_gtimer_cdpm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_cdpm, dnx_data_gtimer_cdpm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ocbm_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ocbm";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ocbm_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ocbm features");

    submodule_data->features[dnx_data_gtimer_ocbm_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ocbm_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ocbm_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ocbm_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ocbm defines");

    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ocbm_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ocbm_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ocbm tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ocbm_feature_get(
    int unit,
    dnx_data_gtimer_ocbm_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocbm, feature);
}


uint32
dnx_data_gtimer_ocbm_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocbm, dnx_data_gtimer_ocbm_define_block_index_size);
}

uint32
dnx_data_gtimer_ocbm_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ocbm, dnx_data_gtimer_ocbm_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_mss_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mss";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_mss_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer mss features");

    submodule_data->features[dnx_data_gtimer_mss_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_mss_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_mss_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_mss_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer mss defines");

    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mss_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_mss_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer mss tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_mss_feature_get(
    int unit,
    dnx_data_gtimer_mss_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mss, feature);
}


uint32
dnx_data_gtimer_mss_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mss, dnx_data_gtimer_mss_define_block_index_size);
}

uint32
dnx_data_gtimer_mss_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mss, dnx_data_gtimer_mss_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_msd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "msd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_msd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer msd features");

    submodule_data->features[dnx_data_gtimer_msd_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_msd_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_msd_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_msd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer msd defines");

    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msd_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_msd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer msd tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_msd_feature_get(
    int unit,
    dnx_data_gtimer_msd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msd, feature);
}


uint32
dnx_data_gtimer_msd_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msd, dnx_data_gtimer_msd_define_block_index_size);
}

uint32
dnx_data_gtimer_msd_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msd, dnx_data_gtimer_msd_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_msw_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "msw";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_msw_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer msw features");

    submodule_data->features[dnx_data_gtimer_msw_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_msw_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_msw_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_msw_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer msw defines");

    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msw_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_msw_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer msw tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_msw_feature_get(
    int unit,
    dnx_data_gtimer_msw_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msw, feature);
}


uint32
dnx_data_gtimer_msw_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msw, dnx_data_gtimer_msw_define_block_index_size);
}

uint32
dnx_data_gtimer_msw_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msw, dnx_data_gtimer_msw_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_arb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "arb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_arb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer arb features");

    submodule_data->features[dnx_data_gtimer_arb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_arb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_arb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_arb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer arb defines");

    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_arb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_arb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer arb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_arb_feature_get(
    int unit,
    dnx_data_gtimer_arb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_arb, feature);
}


uint32
dnx_data_gtimer_arb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_arb, dnx_data_gtimer_arb_define_block_index_size);
}

uint32
dnx_data_gtimer_arb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_arb, dnx_data_gtimer_arb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ofr_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ofr";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ofr_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ofr features");

    submodule_data->features[dnx_data_gtimer_ofr_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ofr_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ofr_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ofr_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ofr defines");

    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ofr_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ofr_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ofr tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ofr_feature_get(
    int unit,
    dnx_data_gtimer_ofr_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ofr, feature);
}


uint32
dnx_data_gtimer_ofr_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ofr, dnx_data_gtimer_ofr_define_block_index_size);
}

uint32
dnx_data_gtimer_ofr_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ofr, dnx_data_gtimer_ofr_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_oft_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "oft";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_oft_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer oft features");

    submodule_data->features[dnx_data_gtimer_oft_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_oft_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_oft_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_oft_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer oft defines");

    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_oft_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_oft_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer oft tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_oft_feature_get(
    int unit,
    dnx_data_gtimer_oft_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_oft, feature);
}


uint32
dnx_data_gtimer_oft_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_oft, dnx_data_gtimer_oft_define_block_index_size);
}

uint32
dnx_data_gtimer_oft_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_oft, dnx_data_gtimer_oft_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_frwa_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "frwa";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_frwa_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer frwa features");

    submodule_data->features[dnx_data_gtimer_frwa_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_frwa_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_frwa_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_frwa_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer frwa defines");

    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_frwa_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_frwa_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer frwa tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_frwa_feature_get(
    int unit,
    dnx_data_gtimer_frwa_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwa, feature);
}


uint32
dnx_data_gtimer_frwa_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwa, dnx_data_gtimer_frwa_define_block_index_size);
}

uint32
dnx_data_gtimer_frwa_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwa, dnx_data_gtimer_frwa_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_frwbc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "frwbc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_frwbc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer frwbc features");

    submodule_data->features[dnx_data_gtimer_frwbc_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_frwbc_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_frwbc_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_frwbc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer frwbc defines");

    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_frwbc_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_frwbc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer frwbc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_frwbc_feature_get(
    int unit,
    dnx_data_gtimer_frwbc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwbc, feature);
}


uint32
dnx_data_gtimer_frwbc_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwbc, dnx_data_gtimer_frwbc_define_block_index_size);
}

uint32
dnx_data_gtimer_frwbc_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_frwbc, dnx_data_gtimer_frwbc_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_wb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "wb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_wb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer wb features");

    submodule_data->features[dnx_data_gtimer_wb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_wb_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_wb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_wb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer wb defines");

    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_wb_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_wb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer wb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_wb_feature_get(
    int unit,
    dnx_data_gtimer_wb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_wb, feature);
}


uint32
dnx_data_gtimer_wb_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_wb, dnx_data_gtimer_wb_define_block_index_size);
}

uint32
dnx_data_gtimer_wb_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_wb, dnx_data_gtimer_wb_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_w40_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "w40";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_w40_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer w40 features");

    submodule_data->features[dnx_data_gtimer_w40_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_w40_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_w40_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_w40_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer w40 defines");

    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_w40_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_w40_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer w40 tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_w40_feature_get(
    int unit,
    dnx_data_gtimer_w40_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_w40, feature);
}


uint32
dnx_data_gtimer_w40_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_w40, dnx_data_gtimer_w40_define_block_index_size);
}

uint32
dnx_data_gtimer_w40_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_w40, dnx_data_gtimer_w40_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_msu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "msu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_msu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer msu features");

    submodule_data->features[dnx_data_gtimer_msu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_msu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_msu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_msu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer msu defines");

    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_msu_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_msu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer msu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_msu_feature_get(
    int unit,
    dnx_data_gtimer_msu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msu, feature);
}


uint32
dnx_data_gtimer_msu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msu, dnx_data_gtimer_msu_define_block_index_size);
}

uint32
dnx_data_gtimer_msu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_msu, dnx_data_gtimer_msu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_pmu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pmu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_pmu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer pmu features");

    submodule_data->features[dnx_data_gtimer_pmu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_pmu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_pmu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_pmu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer pmu defines");

    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_pmu_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_pmu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer pmu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_pmu_feature_get(
    int unit,
    dnx_data_gtimer_pmu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pmu, feature);
}


uint32
dnx_data_gtimer_pmu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pmu, dnx_data_gtimer_pmu_define_block_index_size);
}

uint32
dnx_data_gtimer_pmu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_pmu, dnx_data_gtimer_pmu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_nbu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "nbu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_nbu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer nbu features");

    submodule_data->features[dnx_data_gtimer_nbu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_nbu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_nbu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_nbu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer nbu defines");

    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_nbu_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_nbu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer nbu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_nbu_feature_get(
    int unit,
    dnx_data_gtimer_nbu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nbu, feature);
}


uint32
dnx_data_gtimer_nbu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nbu, dnx_data_gtimer_nbu_define_block_index_size);
}

uint32
dnx_data_gtimer_nbu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_nbu, dnx_data_gtimer_nbu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_mgu_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mgu";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_mgu_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer mgu features");

    submodule_data->features[dnx_data_gtimer_mgu_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_mgu_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_mgu_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_mgu_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer mgu defines");

    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_mgu_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_mgu_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer mgu tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_mgu_feature_get(
    int unit,
    dnx_data_gtimer_mgu_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mgu, feature);
}


uint32
dnx_data_gtimer_mgu_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mgu, dnx_data_gtimer_mgu_define_block_index_size);
}

uint32
dnx_data_gtimer_mgu_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_mgu, dnx_data_gtimer_mgu_define_block_index_max_value);
}










static shr_error_e
dnx_data_gtimer_ipw_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ipw";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_gtimer_ipw_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data gtimer ipw features");

    submodule_data->features[dnx_data_gtimer_ipw_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_gtimer_ipw_is_supported].doc = "";
    submodule_data->features[dnx_data_gtimer_ipw_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_gtimer_ipw_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data gtimer ipw defines");

    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_size].name = "block_index_size";
    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_size].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_max_value].name = "block_index_max_value";
    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_max_value].doc = "";
    
    submodule_data->defines[dnx_data_gtimer_ipw_define_block_index_max_value].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_gtimer_ipw_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data gtimer ipw tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_gtimer_ipw_feature_get(
    int unit,
    dnx_data_gtimer_ipw_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipw, feature);
}


uint32
dnx_data_gtimer_ipw_block_index_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipw, dnx_data_gtimer_ipw_define_block_index_size);
}

uint32
dnx_data_gtimer_ipw_block_index_max_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_gtimer, dnx_data_gtimer_submodule_ipw, dnx_data_gtimer_ipw_define_block_index_max_value);
}







shr_error_e
dnx_data_gtimer_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "gtimer";
    module_data->nof_submodules = _dnx_data_gtimer_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data gtimer submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_gtimer_rtp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_rtp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_fdt_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_fdt]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_fdtl_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_fdtl]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_pem_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_pem]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_dhc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_dhc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cdu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cdu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_esb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_esb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cdum_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cdum]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_mesh_topology_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_mesh_topology]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_bdm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_bdm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_hbc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_hbc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ecgm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ecgm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cfc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cfc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cgm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cgm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_crps_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_crps]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ddha_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ddha]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ddp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ddp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_dqm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_dqm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_edb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_edb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_epni_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_epni]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_epre_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_epre]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_eps_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_eps]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_erpp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_erpp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_etppa_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_etppa]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_etppb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_etppb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_etppc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_etppc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_fqp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_fqp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ile_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ile]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippa_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippa]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippd_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippd]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippe_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippe]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ippf_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ippf]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ips_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ips]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ipt_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ipt]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_iqm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_iqm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ire_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ire]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_itpp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_itpp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_itppd_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_itppd]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_kaps_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_kaps]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_mcp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_mcp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_mrps_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_mrps]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_nmg_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_nmg]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ocb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ocb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_pdm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_pdm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_pqp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_pqp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_rqp_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_rqp]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_sch_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_sch]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_sif_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_sif]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_spb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_spb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_sqm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_sqm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_tcam_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_tcam]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_tdu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_tdu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_dcc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_dcc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cdb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cdb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cdbm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cdbm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_cdpm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_cdpm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ocbm_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ocbm]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_mss_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_mss]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_msd_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_msd]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_msw_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_msw]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_arb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_arb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ofr_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ofr]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_oft_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_oft]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_frwa_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_frwa]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_frwbc_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_frwbc]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_wb_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_wb]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_w40_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_w40]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_msu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_msu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_pmu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_pmu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_nbu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_nbu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_mgu_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_mgu]));
    SHR_IF_ERR_EXIT(dnx_data_gtimer_ipw_init(unit, &module_data->submodules[dnx_data_gtimer_submodule_ipw]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_gtimer_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_gtimer_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_gtimer_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

