

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_GTIMER_H_

#define _DNX_DATA_INTERNAL_GTIMER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_gtimer.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_gtimer_submodule_rtp,
    dnx_data_gtimer_submodule_fdt,
    dnx_data_gtimer_submodule_fdtl,
    dnx_data_gtimer_submodule_pem,
    dnx_data_gtimer_submodule_dhc,
    dnx_data_gtimer_submodule_cdu,
    dnx_data_gtimer_submodule_esb,
    dnx_data_gtimer_submodule_cdum,
    dnx_data_gtimer_submodule_mesh_topology,
    dnx_data_gtimer_submodule_bdm,
    dnx_data_gtimer_submodule_hbc,
    dnx_data_gtimer_submodule_ecgm,
    dnx_data_gtimer_submodule_cfc,
    dnx_data_gtimer_submodule_cgm,
    dnx_data_gtimer_submodule_crps,
    dnx_data_gtimer_submodule_ddha,
    dnx_data_gtimer_submodule_ddp,
    dnx_data_gtimer_submodule_dqm,
    dnx_data_gtimer_submodule_edb,
    dnx_data_gtimer_submodule_epni,
    dnx_data_gtimer_submodule_epre,
    dnx_data_gtimer_submodule_eps,
    dnx_data_gtimer_submodule_erpp,
    dnx_data_gtimer_submodule_etppa,
    dnx_data_gtimer_submodule_etppb,
    dnx_data_gtimer_submodule_etppc,
    dnx_data_gtimer_submodule_fqp,
    dnx_data_gtimer_submodule_ile,
    dnx_data_gtimer_submodule_ippa,
    dnx_data_gtimer_submodule_ippb,
    dnx_data_gtimer_submodule_ippc,
    dnx_data_gtimer_submodule_ippd,
    dnx_data_gtimer_submodule_ippe,
    dnx_data_gtimer_submodule_ippf,
    dnx_data_gtimer_submodule_ips,
    dnx_data_gtimer_submodule_ipt,
    dnx_data_gtimer_submodule_iqm,
    dnx_data_gtimer_submodule_ire,
    dnx_data_gtimer_submodule_itpp,
    dnx_data_gtimer_submodule_itppd,
    dnx_data_gtimer_submodule_kaps,
    dnx_data_gtimer_submodule_mcp,
    dnx_data_gtimer_submodule_mrps,
    dnx_data_gtimer_submodule_nmg,
    dnx_data_gtimer_submodule_ocb,
    dnx_data_gtimer_submodule_pdm,
    dnx_data_gtimer_submodule_pqp,
    dnx_data_gtimer_submodule_rqp,
    dnx_data_gtimer_submodule_sch,
    dnx_data_gtimer_submodule_sif,
    dnx_data_gtimer_submodule_spb,
    dnx_data_gtimer_submodule_sqm,
    dnx_data_gtimer_submodule_tcam,
    dnx_data_gtimer_submodule_tdu,
    dnx_data_gtimer_submodule_dcc,
    dnx_data_gtimer_submodule_cdb,
    dnx_data_gtimer_submodule_cdbm,
    dnx_data_gtimer_submodule_cdpm,
    dnx_data_gtimer_submodule_ocbm,
    dnx_data_gtimer_submodule_mss,
    dnx_data_gtimer_submodule_msd,

    
    _dnx_data_gtimer_submodule_nof
} dnx_data_gtimer_submodule_e;








int dnx_data_gtimer_rtp_feature_get(
    int unit,
    dnx_data_gtimer_rtp_feature_e feature);



typedef enum
{

    
    _dnx_data_gtimer_rtp_define_nof
} dnx_data_gtimer_rtp_define_e;




typedef enum
{

    
    _dnx_data_gtimer_rtp_table_nof
} dnx_data_gtimer_rtp_table_e;









int dnx_data_gtimer_fdt_feature_get(
    int unit,
    dnx_data_gtimer_fdt_feature_e feature);



typedef enum
{

    
    _dnx_data_gtimer_fdt_define_nof
} dnx_data_gtimer_fdt_define_e;




typedef enum
{

    
    _dnx_data_gtimer_fdt_table_nof
} dnx_data_gtimer_fdt_table_e;









int dnx_data_gtimer_fdtl_feature_get(
    int unit,
    dnx_data_gtimer_fdtl_feature_e feature);



typedef enum
{
    dnx_data_gtimer_fdtl_define_block_index_size,
    dnx_data_gtimer_fdtl_define_block_index_max_value,

    
    _dnx_data_gtimer_fdtl_define_nof
} dnx_data_gtimer_fdtl_define_e;



uint32 dnx_data_gtimer_fdtl_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_fdtl_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_fdtl_table_nof
} dnx_data_gtimer_fdtl_table_e;









int dnx_data_gtimer_pem_feature_get(
    int unit,
    dnx_data_gtimer_pem_feature_e feature);



typedef enum
{
    dnx_data_gtimer_pem_define_block_index_size,
    dnx_data_gtimer_pem_define_block_index_max_value,

    
    _dnx_data_gtimer_pem_define_nof
} dnx_data_gtimer_pem_define_e;



uint32 dnx_data_gtimer_pem_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_pem_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_pem_table_nof
} dnx_data_gtimer_pem_table_e;









int dnx_data_gtimer_dhc_feature_get(
    int unit,
    dnx_data_gtimer_dhc_feature_e feature);



typedef enum
{

    
    _dnx_data_gtimer_dhc_define_nof
} dnx_data_gtimer_dhc_define_e;




typedef enum
{

    
    _dnx_data_gtimer_dhc_table_nof
} dnx_data_gtimer_dhc_table_e;









int dnx_data_gtimer_cdu_feature_get(
    int unit,
    dnx_data_gtimer_cdu_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cdu_define_block_index_size,
    dnx_data_gtimer_cdu_define_block_index_max_value,

    
    _dnx_data_gtimer_cdu_define_nof
} dnx_data_gtimer_cdu_define_e;



uint32 dnx_data_gtimer_cdu_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cdu_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cdu_table_nof
} dnx_data_gtimer_cdu_table_e;









int dnx_data_gtimer_esb_feature_get(
    int unit,
    dnx_data_gtimer_esb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_esb_define_block_index_size,
    dnx_data_gtimer_esb_define_block_index_max_value,

    
    _dnx_data_gtimer_esb_define_nof
} dnx_data_gtimer_esb_define_e;



uint32 dnx_data_gtimer_esb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_esb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_esb_table_nof
} dnx_data_gtimer_esb_table_e;









int dnx_data_gtimer_cdum_feature_get(
    int unit,
    dnx_data_gtimer_cdum_feature_e feature);



typedef enum
{

    
    _dnx_data_gtimer_cdum_define_nof
} dnx_data_gtimer_cdum_define_e;




typedef enum
{

    
    _dnx_data_gtimer_cdum_table_nof
} dnx_data_gtimer_cdum_table_e;









int dnx_data_gtimer_mesh_topology_feature_get(
    int unit,
    dnx_data_gtimer_mesh_topology_feature_e feature);



typedef enum
{

    
    _dnx_data_gtimer_mesh_topology_define_nof
} dnx_data_gtimer_mesh_topology_define_e;




typedef enum
{

    
    _dnx_data_gtimer_mesh_topology_table_nof
} dnx_data_gtimer_mesh_topology_table_e;









int dnx_data_gtimer_bdm_feature_get(
    int unit,
    dnx_data_gtimer_bdm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_bdm_define_block_index_size,
    dnx_data_gtimer_bdm_define_block_index_min_value,
    dnx_data_gtimer_bdm_define_block_index_max_value,

    
    _dnx_data_gtimer_bdm_define_nof
} dnx_data_gtimer_bdm_define_e;



uint32 dnx_data_gtimer_bdm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_bdm_block_index_min_value_get(
    int unit);


uint32 dnx_data_gtimer_bdm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_bdm_table_nof
} dnx_data_gtimer_bdm_table_e;









int dnx_data_gtimer_hbc_feature_get(
    int unit,
    dnx_data_gtimer_hbc_feature_e feature);



typedef enum
{
    dnx_data_gtimer_hbc_define_block_index_size,
    dnx_data_gtimer_hbc_define_block_index_max_value,

    
    _dnx_data_gtimer_hbc_define_nof
} dnx_data_gtimer_hbc_define_e;



uint32 dnx_data_gtimer_hbc_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_hbc_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_hbc_table_nof
} dnx_data_gtimer_hbc_table_e;









int dnx_data_gtimer_ecgm_feature_get(
    int unit,
    dnx_data_gtimer_ecgm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ecgm_define_block_index_size,
    dnx_data_gtimer_ecgm_define_block_index_max_value,

    
    _dnx_data_gtimer_ecgm_define_nof
} dnx_data_gtimer_ecgm_define_e;



uint32 dnx_data_gtimer_ecgm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ecgm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ecgm_table_nof
} dnx_data_gtimer_ecgm_table_e;









int dnx_data_gtimer_cfc_feature_get(
    int unit,
    dnx_data_gtimer_cfc_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cfc_define_block_index_size,
    dnx_data_gtimer_cfc_define_block_index_max_value,

    
    _dnx_data_gtimer_cfc_define_nof
} dnx_data_gtimer_cfc_define_e;



uint32 dnx_data_gtimer_cfc_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cfc_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cfc_table_nof
} dnx_data_gtimer_cfc_table_e;









int dnx_data_gtimer_cgm_feature_get(
    int unit,
    dnx_data_gtimer_cgm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cgm_define_block_index_size,
    dnx_data_gtimer_cgm_define_block_index_max_value,

    
    _dnx_data_gtimer_cgm_define_nof
} dnx_data_gtimer_cgm_define_e;



uint32 dnx_data_gtimer_cgm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cgm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cgm_table_nof
} dnx_data_gtimer_cgm_table_e;









int dnx_data_gtimer_crps_feature_get(
    int unit,
    dnx_data_gtimer_crps_feature_e feature);



typedef enum
{
    dnx_data_gtimer_crps_define_block_index_size,
    dnx_data_gtimer_crps_define_block_index_max_value,

    
    _dnx_data_gtimer_crps_define_nof
} dnx_data_gtimer_crps_define_e;



uint32 dnx_data_gtimer_crps_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_crps_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_crps_table_nof
} dnx_data_gtimer_crps_table_e;









int dnx_data_gtimer_ddha_feature_get(
    int unit,
    dnx_data_gtimer_ddha_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ddha_define_block_index_size,
    dnx_data_gtimer_ddha_define_block_index_max_value,

    
    _dnx_data_gtimer_ddha_define_nof
} dnx_data_gtimer_ddha_define_e;



uint32 dnx_data_gtimer_ddha_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ddha_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ddha_table_nof
} dnx_data_gtimer_ddha_table_e;









int dnx_data_gtimer_ddp_feature_get(
    int unit,
    dnx_data_gtimer_ddp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ddp_define_block_index_size,
    dnx_data_gtimer_ddp_define_block_index_max_value,

    
    _dnx_data_gtimer_ddp_define_nof
} dnx_data_gtimer_ddp_define_e;



uint32 dnx_data_gtimer_ddp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ddp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ddp_table_nof
} dnx_data_gtimer_ddp_table_e;









int dnx_data_gtimer_dqm_feature_get(
    int unit,
    dnx_data_gtimer_dqm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_dqm_define_block_index_size,
    dnx_data_gtimer_dqm_define_block_index_max_value,

    
    _dnx_data_gtimer_dqm_define_nof
} dnx_data_gtimer_dqm_define_e;



uint32 dnx_data_gtimer_dqm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_dqm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_dqm_table_nof
} dnx_data_gtimer_dqm_table_e;









int dnx_data_gtimer_edb_feature_get(
    int unit,
    dnx_data_gtimer_edb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_edb_define_block_index_size,
    dnx_data_gtimer_edb_define_block_index_max_value,

    
    _dnx_data_gtimer_edb_define_nof
} dnx_data_gtimer_edb_define_e;



uint32 dnx_data_gtimer_edb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_edb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_edb_table_nof
} dnx_data_gtimer_edb_table_e;









int dnx_data_gtimer_epni_feature_get(
    int unit,
    dnx_data_gtimer_epni_feature_e feature);



typedef enum
{
    dnx_data_gtimer_epni_define_block_index_size,
    dnx_data_gtimer_epni_define_block_index_max_value,

    
    _dnx_data_gtimer_epni_define_nof
} dnx_data_gtimer_epni_define_e;



uint32 dnx_data_gtimer_epni_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_epni_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_epni_table_nof
} dnx_data_gtimer_epni_table_e;









int dnx_data_gtimer_epre_feature_get(
    int unit,
    dnx_data_gtimer_epre_feature_e feature);



typedef enum
{
    dnx_data_gtimer_epre_define_block_index_size,
    dnx_data_gtimer_epre_define_block_index_max_value,

    
    _dnx_data_gtimer_epre_define_nof
} dnx_data_gtimer_epre_define_e;



uint32 dnx_data_gtimer_epre_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_epre_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_epre_table_nof
} dnx_data_gtimer_epre_table_e;









int dnx_data_gtimer_eps_feature_get(
    int unit,
    dnx_data_gtimer_eps_feature_e feature);



typedef enum
{
    dnx_data_gtimer_eps_define_block_index_size,
    dnx_data_gtimer_eps_define_block_index_max_value,

    
    _dnx_data_gtimer_eps_define_nof
} dnx_data_gtimer_eps_define_e;



uint32 dnx_data_gtimer_eps_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_eps_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_eps_table_nof
} dnx_data_gtimer_eps_table_e;









int dnx_data_gtimer_erpp_feature_get(
    int unit,
    dnx_data_gtimer_erpp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_erpp_define_block_index_size,
    dnx_data_gtimer_erpp_define_block_index_max_value,

    
    _dnx_data_gtimer_erpp_define_nof
} dnx_data_gtimer_erpp_define_e;



uint32 dnx_data_gtimer_erpp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_erpp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_erpp_table_nof
} dnx_data_gtimer_erpp_table_e;









int dnx_data_gtimer_etppa_feature_get(
    int unit,
    dnx_data_gtimer_etppa_feature_e feature);



typedef enum
{
    dnx_data_gtimer_etppa_define_block_index_size,
    dnx_data_gtimer_etppa_define_block_index_max_value,

    
    _dnx_data_gtimer_etppa_define_nof
} dnx_data_gtimer_etppa_define_e;



uint32 dnx_data_gtimer_etppa_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_etppa_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_etppa_table_nof
} dnx_data_gtimer_etppa_table_e;









int dnx_data_gtimer_etppb_feature_get(
    int unit,
    dnx_data_gtimer_etppb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_etppb_define_block_index_size,
    dnx_data_gtimer_etppb_define_block_index_max_value,

    
    _dnx_data_gtimer_etppb_define_nof
} dnx_data_gtimer_etppb_define_e;



uint32 dnx_data_gtimer_etppb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_etppb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_etppb_table_nof
} dnx_data_gtimer_etppb_table_e;









int dnx_data_gtimer_etppc_feature_get(
    int unit,
    dnx_data_gtimer_etppc_feature_e feature);



typedef enum
{
    dnx_data_gtimer_etppc_define_block_index_size,
    dnx_data_gtimer_etppc_define_block_index_max_value,

    
    _dnx_data_gtimer_etppc_define_nof
} dnx_data_gtimer_etppc_define_e;



uint32 dnx_data_gtimer_etppc_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_etppc_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_etppc_table_nof
} dnx_data_gtimer_etppc_table_e;









int dnx_data_gtimer_fqp_feature_get(
    int unit,
    dnx_data_gtimer_fqp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_fqp_define_block_index_size,
    dnx_data_gtimer_fqp_define_block_index_max_value,

    
    _dnx_data_gtimer_fqp_define_nof
} dnx_data_gtimer_fqp_define_e;



uint32 dnx_data_gtimer_fqp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_fqp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_fqp_table_nof
} dnx_data_gtimer_fqp_table_e;









int dnx_data_gtimer_ile_feature_get(
    int unit,
    dnx_data_gtimer_ile_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ile_define_block_index_size,
    dnx_data_gtimer_ile_define_block_index_max_value,

    
    _dnx_data_gtimer_ile_define_nof
} dnx_data_gtimer_ile_define_e;



uint32 dnx_data_gtimer_ile_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ile_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ile_table_nof
} dnx_data_gtimer_ile_table_e;









int dnx_data_gtimer_ippa_feature_get(
    int unit,
    dnx_data_gtimer_ippa_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippa_define_block_index_size,
    dnx_data_gtimer_ippa_define_block_index_max_value,

    
    _dnx_data_gtimer_ippa_define_nof
} dnx_data_gtimer_ippa_define_e;



uint32 dnx_data_gtimer_ippa_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippa_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippa_table_nof
} dnx_data_gtimer_ippa_table_e;









int dnx_data_gtimer_ippb_feature_get(
    int unit,
    dnx_data_gtimer_ippb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippb_define_block_index_size,
    dnx_data_gtimer_ippb_define_block_index_max_value,

    
    _dnx_data_gtimer_ippb_define_nof
} dnx_data_gtimer_ippb_define_e;



uint32 dnx_data_gtimer_ippb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippb_table_nof
} dnx_data_gtimer_ippb_table_e;









int dnx_data_gtimer_ippc_feature_get(
    int unit,
    dnx_data_gtimer_ippc_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippc_define_block_index_size,
    dnx_data_gtimer_ippc_define_block_index_max_value,

    
    _dnx_data_gtimer_ippc_define_nof
} dnx_data_gtimer_ippc_define_e;



uint32 dnx_data_gtimer_ippc_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippc_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippc_table_nof
} dnx_data_gtimer_ippc_table_e;









int dnx_data_gtimer_ippd_feature_get(
    int unit,
    dnx_data_gtimer_ippd_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippd_define_block_index_size,
    dnx_data_gtimer_ippd_define_block_index_max_value,

    
    _dnx_data_gtimer_ippd_define_nof
} dnx_data_gtimer_ippd_define_e;



uint32 dnx_data_gtimer_ippd_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippd_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippd_table_nof
} dnx_data_gtimer_ippd_table_e;









int dnx_data_gtimer_ippe_feature_get(
    int unit,
    dnx_data_gtimer_ippe_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippe_define_block_index_size,
    dnx_data_gtimer_ippe_define_block_index_max_value,

    
    _dnx_data_gtimer_ippe_define_nof
} dnx_data_gtimer_ippe_define_e;



uint32 dnx_data_gtimer_ippe_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippe_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippe_table_nof
} dnx_data_gtimer_ippe_table_e;









int dnx_data_gtimer_ippf_feature_get(
    int unit,
    dnx_data_gtimer_ippf_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ippf_define_block_index_size,
    dnx_data_gtimer_ippf_define_block_index_max_value,

    
    _dnx_data_gtimer_ippf_define_nof
} dnx_data_gtimer_ippf_define_e;



uint32 dnx_data_gtimer_ippf_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ippf_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ippf_table_nof
} dnx_data_gtimer_ippf_table_e;









int dnx_data_gtimer_ips_feature_get(
    int unit,
    dnx_data_gtimer_ips_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ips_define_block_index_size,
    dnx_data_gtimer_ips_define_block_index_max_value,

    
    _dnx_data_gtimer_ips_define_nof
} dnx_data_gtimer_ips_define_e;



uint32 dnx_data_gtimer_ips_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ips_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ips_table_nof
} dnx_data_gtimer_ips_table_e;









int dnx_data_gtimer_ipt_feature_get(
    int unit,
    dnx_data_gtimer_ipt_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ipt_define_block_index_size,
    dnx_data_gtimer_ipt_define_block_index_max_value,

    
    _dnx_data_gtimer_ipt_define_nof
} dnx_data_gtimer_ipt_define_e;



uint32 dnx_data_gtimer_ipt_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ipt_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ipt_table_nof
} dnx_data_gtimer_ipt_table_e;









int dnx_data_gtimer_iqm_feature_get(
    int unit,
    dnx_data_gtimer_iqm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_iqm_define_block_index_size,
    dnx_data_gtimer_iqm_define_block_index_max_value,

    
    _dnx_data_gtimer_iqm_define_nof
} dnx_data_gtimer_iqm_define_e;



uint32 dnx_data_gtimer_iqm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_iqm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_iqm_table_nof
} dnx_data_gtimer_iqm_table_e;









int dnx_data_gtimer_ire_feature_get(
    int unit,
    dnx_data_gtimer_ire_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ire_define_block_index_size,
    dnx_data_gtimer_ire_define_block_index_max_value,

    
    _dnx_data_gtimer_ire_define_nof
} dnx_data_gtimer_ire_define_e;



uint32 dnx_data_gtimer_ire_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ire_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ire_table_nof
} dnx_data_gtimer_ire_table_e;









int dnx_data_gtimer_itpp_feature_get(
    int unit,
    dnx_data_gtimer_itpp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_itpp_define_block_index_size,
    dnx_data_gtimer_itpp_define_block_index_max_value,

    
    _dnx_data_gtimer_itpp_define_nof
} dnx_data_gtimer_itpp_define_e;



uint32 dnx_data_gtimer_itpp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_itpp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_itpp_table_nof
} dnx_data_gtimer_itpp_table_e;









int dnx_data_gtimer_itppd_feature_get(
    int unit,
    dnx_data_gtimer_itppd_feature_e feature);



typedef enum
{
    dnx_data_gtimer_itppd_define_block_index_size,
    dnx_data_gtimer_itppd_define_block_index_max_value,

    
    _dnx_data_gtimer_itppd_define_nof
} dnx_data_gtimer_itppd_define_e;



uint32 dnx_data_gtimer_itppd_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_itppd_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_itppd_table_nof
} dnx_data_gtimer_itppd_table_e;









int dnx_data_gtimer_kaps_feature_get(
    int unit,
    dnx_data_gtimer_kaps_feature_e feature);



typedef enum
{
    dnx_data_gtimer_kaps_define_block_index_size,
    dnx_data_gtimer_kaps_define_block_index_max_value,

    
    _dnx_data_gtimer_kaps_define_nof
} dnx_data_gtimer_kaps_define_e;



uint32 dnx_data_gtimer_kaps_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_kaps_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_kaps_table_nof
} dnx_data_gtimer_kaps_table_e;









int dnx_data_gtimer_mcp_feature_get(
    int unit,
    dnx_data_gtimer_mcp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_mcp_define_block_index_size,
    dnx_data_gtimer_mcp_define_block_index_max_value,

    
    _dnx_data_gtimer_mcp_define_nof
} dnx_data_gtimer_mcp_define_e;



uint32 dnx_data_gtimer_mcp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_mcp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_mcp_table_nof
} dnx_data_gtimer_mcp_table_e;









int dnx_data_gtimer_mrps_feature_get(
    int unit,
    dnx_data_gtimer_mrps_feature_e feature);



typedef enum
{
    dnx_data_gtimer_mrps_define_block_index_size,
    dnx_data_gtimer_mrps_define_block_index_max_value,

    
    _dnx_data_gtimer_mrps_define_nof
} dnx_data_gtimer_mrps_define_e;



uint32 dnx_data_gtimer_mrps_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_mrps_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_mrps_table_nof
} dnx_data_gtimer_mrps_table_e;









int dnx_data_gtimer_nmg_feature_get(
    int unit,
    dnx_data_gtimer_nmg_feature_e feature);



typedef enum
{
    dnx_data_gtimer_nmg_define_block_index_size,
    dnx_data_gtimer_nmg_define_block_index_max_value,

    
    _dnx_data_gtimer_nmg_define_nof
} dnx_data_gtimer_nmg_define_e;



uint32 dnx_data_gtimer_nmg_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_nmg_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_nmg_table_nof
} dnx_data_gtimer_nmg_table_e;









int dnx_data_gtimer_ocb_feature_get(
    int unit,
    dnx_data_gtimer_ocb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ocb_define_block_index_size,
    dnx_data_gtimer_ocb_define_block_index_max_value,

    
    _dnx_data_gtimer_ocb_define_nof
} dnx_data_gtimer_ocb_define_e;



uint32 dnx_data_gtimer_ocb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ocb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ocb_table_nof
} dnx_data_gtimer_ocb_table_e;









int dnx_data_gtimer_pdm_feature_get(
    int unit,
    dnx_data_gtimer_pdm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_pdm_define_block_index_size,
    dnx_data_gtimer_pdm_define_block_index_max_value,

    
    _dnx_data_gtimer_pdm_define_nof
} dnx_data_gtimer_pdm_define_e;



uint32 dnx_data_gtimer_pdm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_pdm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_pdm_table_nof
} dnx_data_gtimer_pdm_table_e;









int dnx_data_gtimer_pqp_feature_get(
    int unit,
    dnx_data_gtimer_pqp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_pqp_define_block_index_size,
    dnx_data_gtimer_pqp_define_block_index_max_value,

    
    _dnx_data_gtimer_pqp_define_nof
} dnx_data_gtimer_pqp_define_e;



uint32 dnx_data_gtimer_pqp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_pqp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_pqp_table_nof
} dnx_data_gtimer_pqp_table_e;









int dnx_data_gtimer_rqp_feature_get(
    int unit,
    dnx_data_gtimer_rqp_feature_e feature);



typedef enum
{
    dnx_data_gtimer_rqp_define_block_index_size,
    dnx_data_gtimer_rqp_define_block_index_max_value,

    
    _dnx_data_gtimer_rqp_define_nof
} dnx_data_gtimer_rqp_define_e;



uint32 dnx_data_gtimer_rqp_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_rqp_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_rqp_table_nof
} dnx_data_gtimer_rqp_table_e;









int dnx_data_gtimer_sch_feature_get(
    int unit,
    dnx_data_gtimer_sch_feature_e feature);



typedef enum
{
    dnx_data_gtimer_sch_define_block_index_size,
    dnx_data_gtimer_sch_define_block_index_max_value,

    
    _dnx_data_gtimer_sch_define_nof
} dnx_data_gtimer_sch_define_e;



uint32 dnx_data_gtimer_sch_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_sch_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_sch_table_nof
} dnx_data_gtimer_sch_table_e;









int dnx_data_gtimer_sif_feature_get(
    int unit,
    dnx_data_gtimer_sif_feature_e feature);



typedef enum
{
    dnx_data_gtimer_sif_define_block_index_size,
    dnx_data_gtimer_sif_define_block_index_max_value,

    
    _dnx_data_gtimer_sif_define_nof
} dnx_data_gtimer_sif_define_e;



uint32 dnx_data_gtimer_sif_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_sif_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_sif_table_nof
} dnx_data_gtimer_sif_table_e;









int dnx_data_gtimer_spb_feature_get(
    int unit,
    dnx_data_gtimer_spb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_spb_define_block_index_size,
    dnx_data_gtimer_spb_define_block_index_max_value,

    
    _dnx_data_gtimer_spb_define_nof
} dnx_data_gtimer_spb_define_e;



uint32 dnx_data_gtimer_spb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_spb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_spb_table_nof
} dnx_data_gtimer_spb_table_e;









int dnx_data_gtimer_sqm_feature_get(
    int unit,
    dnx_data_gtimer_sqm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_sqm_define_block_index_size,
    dnx_data_gtimer_sqm_define_block_index_max_value,

    
    _dnx_data_gtimer_sqm_define_nof
} dnx_data_gtimer_sqm_define_e;



uint32 dnx_data_gtimer_sqm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_sqm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_sqm_table_nof
} dnx_data_gtimer_sqm_table_e;









int dnx_data_gtimer_tcam_feature_get(
    int unit,
    dnx_data_gtimer_tcam_feature_e feature);



typedef enum
{
    dnx_data_gtimer_tcam_define_block_index_size,
    dnx_data_gtimer_tcam_define_block_index_max_value,

    
    _dnx_data_gtimer_tcam_define_nof
} dnx_data_gtimer_tcam_define_e;



uint32 dnx_data_gtimer_tcam_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_tcam_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_tcam_table_nof
} dnx_data_gtimer_tcam_table_e;









int dnx_data_gtimer_tdu_feature_get(
    int unit,
    dnx_data_gtimer_tdu_feature_e feature);



typedef enum
{
    dnx_data_gtimer_tdu_define_block_index_size,
    dnx_data_gtimer_tdu_define_block_index_max_value,

    
    _dnx_data_gtimer_tdu_define_nof
} dnx_data_gtimer_tdu_define_e;



uint32 dnx_data_gtimer_tdu_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_tdu_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_tdu_table_nof
} dnx_data_gtimer_tdu_table_e;









int dnx_data_gtimer_dcc_feature_get(
    int unit,
    dnx_data_gtimer_dcc_feature_e feature);



typedef enum
{
    dnx_data_gtimer_dcc_define_block_index_size,
    dnx_data_gtimer_dcc_define_block_index_max_value,

    
    _dnx_data_gtimer_dcc_define_nof
} dnx_data_gtimer_dcc_define_e;



uint32 dnx_data_gtimer_dcc_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_dcc_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_dcc_table_nof
} dnx_data_gtimer_dcc_table_e;









int dnx_data_gtimer_cdb_feature_get(
    int unit,
    dnx_data_gtimer_cdb_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cdb_define_block_index_size,
    dnx_data_gtimer_cdb_define_block_index_max_value,

    
    _dnx_data_gtimer_cdb_define_nof
} dnx_data_gtimer_cdb_define_e;



uint32 dnx_data_gtimer_cdb_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cdb_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cdb_table_nof
} dnx_data_gtimer_cdb_table_e;









int dnx_data_gtimer_cdbm_feature_get(
    int unit,
    dnx_data_gtimer_cdbm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cdbm_define_block_index_size,
    dnx_data_gtimer_cdbm_define_block_index_max_value,

    
    _dnx_data_gtimer_cdbm_define_nof
} dnx_data_gtimer_cdbm_define_e;



uint32 dnx_data_gtimer_cdbm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cdbm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cdbm_table_nof
} dnx_data_gtimer_cdbm_table_e;









int dnx_data_gtimer_cdpm_feature_get(
    int unit,
    dnx_data_gtimer_cdpm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_cdpm_define_block_index_size,
    dnx_data_gtimer_cdpm_define_block_index_max_value,

    
    _dnx_data_gtimer_cdpm_define_nof
} dnx_data_gtimer_cdpm_define_e;



uint32 dnx_data_gtimer_cdpm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_cdpm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_cdpm_table_nof
} dnx_data_gtimer_cdpm_table_e;









int dnx_data_gtimer_ocbm_feature_get(
    int unit,
    dnx_data_gtimer_ocbm_feature_e feature);



typedef enum
{
    dnx_data_gtimer_ocbm_define_block_index_size,
    dnx_data_gtimer_ocbm_define_block_index_max_value,

    
    _dnx_data_gtimer_ocbm_define_nof
} dnx_data_gtimer_ocbm_define_e;



uint32 dnx_data_gtimer_ocbm_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_ocbm_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_ocbm_table_nof
} dnx_data_gtimer_ocbm_table_e;









int dnx_data_gtimer_mss_feature_get(
    int unit,
    dnx_data_gtimer_mss_feature_e feature);



typedef enum
{
    dnx_data_gtimer_mss_define_block_index_size,
    dnx_data_gtimer_mss_define_block_index_max_value,

    
    _dnx_data_gtimer_mss_define_nof
} dnx_data_gtimer_mss_define_e;



uint32 dnx_data_gtimer_mss_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_mss_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_mss_table_nof
} dnx_data_gtimer_mss_table_e;









int dnx_data_gtimer_msd_feature_get(
    int unit,
    dnx_data_gtimer_msd_feature_e feature);



typedef enum
{
    dnx_data_gtimer_msd_define_block_index_size,
    dnx_data_gtimer_msd_define_block_index_max_value,

    
    _dnx_data_gtimer_msd_define_nof
} dnx_data_gtimer_msd_define_e;



uint32 dnx_data_gtimer_msd_block_index_size_get(
    int unit);


uint32 dnx_data_gtimer_msd_block_index_max_value_get(
    int unit);



typedef enum
{

    
    _dnx_data_gtimer_msd_table_nof
} dnx_data_gtimer_msd_table_e;






shr_error_e dnx_data_gtimer_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

