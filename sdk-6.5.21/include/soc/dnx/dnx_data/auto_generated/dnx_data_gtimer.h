

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_GTIMER_H_

#define _DNX_DATA_GTIMER_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_gtimer.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_gtimer_init(
    int unit);







typedef enum
{
    
    dnx_data_gtimer_rtp_is_supported,

    
    _dnx_data_gtimer_rtp_feature_nof
} dnx_data_gtimer_rtp_feature_e;



typedef int(
    *dnx_data_gtimer_rtp_feature_get_f) (
    int unit,
    dnx_data_gtimer_rtp_feature_e feature);



typedef struct
{
    
    dnx_data_gtimer_rtp_feature_get_f feature_get;
} dnx_data_if_gtimer_rtp_t;







typedef enum
{
    
    dnx_data_gtimer_fdt_is_supported,

    
    _dnx_data_gtimer_fdt_feature_nof
} dnx_data_gtimer_fdt_feature_e;



typedef int(
    *dnx_data_gtimer_fdt_feature_get_f) (
    int unit,
    dnx_data_gtimer_fdt_feature_e feature);



typedef struct
{
    
    dnx_data_gtimer_fdt_feature_get_f feature_get;
} dnx_data_if_gtimer_fdt_t;







typedef enum
{
    
    dnx_data_gtimer_fdtl_is_supported,

    
    _dnx_data_gtimer_fdtl_feature_nof
} dnx_data_gtimer_fdtl_feature_e;



typedef int(
    *dnx_data_gtimer_fdtl_feature_get_f) (
    int unit,
    dnx_data_gtimer_fdtl_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_fdtl_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_fdtl_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_fdtl_feature_get_f feature_get;
    
    dnx_data_gtimer_fdtl_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_fdtl_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_fdtl_t;







typedef enum
{
    
    dnx_data_gtimer_pem_is_supported,

    
    _dnx_data_gtimer_pem_feature_nof
} dnx_data_gtimer_pem_feature_e;



typedef int(
    *dnx_data_gtimer_pem_feature_get_f) (
    int unit,
    dnx_data_gtimer_pem_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_pem_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_pem_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_pem_feature_get_f feature_get;
    
    dnx_data_gtimer_pem_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_pem_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_pem_t;







typedef enum
{
    
    dnx_data_gtimer_dhc_is_supported,

    
    _dnx_data_gtimer_dhc_feature_nof
} dnx_data_gtimer_dhc_feature_e;



typedef int(
    *dnx_data_gtimer_dhc_feature_get_f) (
    int unit,
    dnx_data_gtimer_dhc_feature_e feature);



typedef struct
{
    
    dnx_data_gtimer_dhc_feature_get_f feature_get;
} dnx_data_if_gtimer_dhc_t;







typedef enum
{
    
    dnx_data_gtimer_cdu_is_supported,

    
    _dnx_data_gtimer_cdu_feature_nof
} dnx_data_gtimer_cdu_feature_e;



typedef int(
    *dnx_data_gtimer_cdu_feature_get_f) (
    int unit,
    dnx_data_gtimer_cdu_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cdu_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cdu_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cdu_feature_get_f feature_get;
    
    dnx_data_gtimer_cdu_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cdu_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cdu_t;







typedef enum
{
    
    dnx_data_gtimer_esb_is_supported,

    
    _dnx_data_gtimer_esb_feature_nof
} dnx_data_gtimer_esb_feature_e;



typedef int(
    *dnx_data_gtimer_esb_feature_get_f) (
    int unit,
    dnx_data_gtimer_esb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_esb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_esb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_esb_feature_get_f feature_get;
    
    dnx_data_gtimer_esb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_esb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_esb_t;







typedef enum
{
    
    dnx_data_gtimer_cdum_is_supported,

    
    _dnx_data_gtimer_cdum_feature_nof
} dnx_data_gtimer_cdum_feature_e;



typedef int(
    *dnx_data_gtimer_cdum_feature_get_f) (
    int unit,
    dnx_data_gtimer_cdum_feature_e feature);



typedef struct
{
    
    dnx_data_gtimer_cdum_feature_get_f feature_get;
} dnx_data_if_gtimer_cdum_t;







typedef enum
{
    
    dnx_data_gtimer_mesh_topology_is_supported,

    
    _dnx_data_gtimer_mesh_topology_feature_nof
} dnx_data_gtimer_mesh_topology_feature_e;



typedef int(
    *dnx_data_gtimer_mesh_topology_feature_get_f) (
    int unit,
    dnx_data_gtimer_mesh_topology_feature_e feature);



typedef struct
{
    
    dnx_data_gtimer_mesh_topology_feature_get_f feature_get;
} dnx_data_if_gtimer_mesh_topology_t;







typedef enum
{
    
    dnx_data_gtimer_bdm_is_supported,

    
    _dnx_data_gtimer_bdm_feature_nof
} dnx_data_gtimer_bdm_feature_e;



typedef int(
    *dnx_data_gtimer_bdm_feature_get_f) (
    int unit,
    dnx_data_gtimer_bdm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_bdm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_bdm_block_index_min_value_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_bdm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_bdm_feature_get_f feature_get;
    
    dnx_data_gtimer_bdm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_bdm_block_index_min_value_get_f block_index_min_value_get;
    
    dnx_data_gtimer_bdm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_bdm_t;







typedef enum
{

    
    _dnx_data_gtimer_hbc_feature_nof
} dnx_data_gtimer_hbc_feature_e;



typedef int(
    *dnx_data_gtimer_hbc_feature_get_f) (
    int unit,
    dnx_data_gtimer_hbc_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_hbc_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_hbc_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_hbc_feature_get_f feature_get;
    
    dnx_data_gtimer_hbc_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_hbc_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_hbc_t;







typedef enum
{
    
    dnx_data_gtimer_ecgm_is_supported,

    
    _dnx_data_gtimer_ecgm_feature_nof
} dnx_data_gtimer_ecgm_feature_e;



typedef int(
    *dnx_data_gtimer_ecgm_feature_get_f) (
    int unit,
    dnx_data_gtimer_ecgm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ecgm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ecgm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ecgm_feature_get_f feature_get;
    
    dnx_data_gtimer_ecgm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ecgm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ecgm_t;







typedef enum
{
    
    dnx_data_gtimer_cfc_is_supported,

    
    _dnx_data_gtimer_cfc_feature_nof
} dnx_data_gtimer_cfc_feature_e;



typedef int(
    *dnx_data_gtimer_cfc_feature_get_f) (
    int unit,
    dnx_data_gtimer_cfc_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cfc_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cfc_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cfc_feature_get_f feature_get;
    
    dnx_data_gtimer_cfc_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cfc_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cfc_t;







typedef enum
{
    
    dnx_data_gtimer_cgm_is_supported,

    
    _dnx_data_gtimer_cgm_feature_nof
} dnx_data_gtimer_cgm_feature_e;



typedef int(
    *dnx_data_gtimer_cgm_feature_get_f) (
    int unit,
    dnx_data_gtimer_cgm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cgm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cgm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cgm_feature_get_f feature_get;
    
    dnx_data_gtimer_cgm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cgm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cgm_t;







typedef enum
{
    
    dnx_data_gtimer_crps_is_supported,

    
    _dnx_data_gtimer_crps_feature_nof
} dnx_data_gtimer_crps_feature_e;



typedef int(
    *dnx_data_gtimer_crps_feature_get_f) (
    int unit,
    dnx_data_gtimer_crps_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_crps_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_crps_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_crps_feature_get_f feature_get;
    
    dnx_data_gtimer_crps_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_crps_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_crps_t;







typedef enum
{
    
    dnx_data_gtimer_ddha_is_supported,

    
    _dnx_data_gtimer_ddha_feature_nof
} dnx_data_gtimer_ddha_feature_e;



typedef int(
    *dnx_data_gtimer_ddha_feature_get_f) (
    int unit,
    dnx_data_gtimer_ddha_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ddha_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ddha_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ddha_feature_get_f feature_get;
    
    dnx_data_gtimer_ddha_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ddha_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ddha_t;







typedef enum
{
    
    dnx_data_gtimer_ddp_is_supported,

    
    _dnx_data_gtimer_ddp_feature_nof
} dnx_data_gtimer_ddp_feature_e;



typedef int(
    *dnx_data_gtimer_ddp_feature_get_f) (
    int unit,
    dnx_data_gtimer_ddp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ddp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ddp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ddp_feature_get_f feature_get;
    
    dnx_data_gtimer_ddp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ddp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ddp_t;







typedef enum
{
    
    dnx_data_gtimer_dqm_is_supported,

    
    _dnx_data_gtimer_dqm_feature_nof
} dnx_data_gtimer_dqm_feature_e;



typedef int(
    *dnx_data_gtimer_dqm_feature_get_f) (
    int unit,
    dnx_data_gtimer_dqm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_dqm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_dqm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_dqm_feature_get_f feature_get;
    
    dnx_data_gtimer_dqm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_dqm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_dqm_t;







typedef enum
{
    
    dnx_data_gtimer_edb_is_supported,

    
    _dnx_data_gtimer_edb_feature_nof
} dnx_data_gtimer_edb_feature_e;



typedef int(
    *dnx_data_gtimer_edb_feature_get_f) (
    int unit,
    dnx_data_gtimer_edb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_edb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_edb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_edb_feature_get_f feature_get;
    
    dnx_data_gtimer_edb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_edb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_edb_t;







typedef enum
{
    
    dnx_data_gtimer_epni_is_supported,

    
    _dnx_data_gtimer_epni_feature_nof
} dnx_data_gtimer_epni_feature_e;



typedef int(
    *dnx_data_gtimer_epni_feature_get_f) (
    int unit,
    dnx_data_gtimer_epni_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_epni_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_epni_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_epni_feature_get_f feature_get;
    
    dnx_data_gtimer_epni_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_epni_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_epni_t;







typedef enum
{
    
    dnx_data_gtimer_epre_is_supported,

    
    _dnx_data_gtimer_epre_feature_nof
} dnx_data_gtimer_epre_feature_e;



typedef int(
    *dnx_data_gtimer_epre_feature_get_f) (
    int unit,
    dnx_data_gtimer_epre_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_epre_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_epre_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_epre_feature_get_f feature_get;
    
    dnx_data_gtimer_epre_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_epre_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_epre_t;







typedef enum
{
    
    dnx_data_gtimer_eps_is_supported,

    
    _dnx_data_gtimer_eps_feature_nof
} dnx_data_gtimer_eps_feature_e;



typedef int(
    *dnx_data_gtimer_eps_feature_get_f) (
    int unit,
    dnx_data_gtimer_eps_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_eps_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_eps_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_eps_feature_get_f feature_get;
    
    dnx_data_gtimer_eps_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_eps_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_eps_t;







typedef enum
{
    
    dnx_data_gtimer_erpp_is_supported,

    
    _dnx_data_gtimer_erpp_feature_nof
} dnx_data_gtimer_erpp_feature_e;



typedef int(
    *dnx_data_gtimer_erpp_feature_get_f) (
    int unit,
    dnx_data_gtimer_erpp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_erpp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_erpp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_erpp_feature_get_f feature_get;
    
    dnx_data_gtimer_erpp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_erpp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_erpp_t;







typedef enum
{
    
    dnx_data_gtimer_etppa_is_supported,

    
    _dnx_data_gtimer_etppa_feature_nof
} dnx_data_gtimer_etppa_feature_e;



typedef int(
    *dnx_data_gtimer_etppa_feature_get_f) (
    int unit,
    dnx_data_gtimer_etppa_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_etppa_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_etppa_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_etppa_feature_get_f feature_get;
    
    dnx_data_gtimer_etppa_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_etppa_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_etppa_t;







typedef enum
{
    
    dnx_data_gtimer_etppb_is_supported,

    
    _dnx_data_gtimer_etppb_feature_nof
} dnx_data_gtimer_etppb_feature_e;



typedef int(
    *dnx_data_gtimer_etppb_feature_get_f) (
    int unit,
    dnx_data_gtimer_etppb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_etppb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_etppb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_etppb_feature_get_f feature_get;
    
    dnx_data_gtimer_etppb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_etppb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_etppb_t;







typedef enum
{
    
    dnx_data_gtimer_etppc_is_supported,

    
    _dnx_data_gtimer_etppc_feature_nof
} dnx_data_gtimer_etppc_feature_e;



typedef int(
    *dnx_data_gtimer_etppc_feature_get_f) (
    int unit,
    dnx_data_gtimer_etppc_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_etppc_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_etppc_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_etppc_feature_get_f feature_get;
    
    dnx_data_gtimer_etppc_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_etppc_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_etppc_t;







typedef enum
{
    
    dnx_data_gtimer_fqp_is_supported,

    
    _dnx_data_gtimer_fqp_feature_nof
} dnx_data_gtimer_fqp_feature_e;



typedef int(
    *dnx_data_gtimer_fqp_feature_get_f) (
    int unit,
    dnx_data_gtimer_fqp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_fqp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_fqp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_fqp_feature_get_f feature_get;
    
    dnx_data_gtimer_fqp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_fqp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_fqp_t;







typedef enum
{
    
    dnx_data_gtimer_ile_is_supported,

    
    _dnx_data_gtimer_ile_feature_nof
} dnx_data_gtimer_ile_feature_e;



typedef int(
    *dnx_data_gtimer_ile_feature_get_f) (
    int unit,
    dnx_data_gtimer_ile_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ile_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ile_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ile_feature_get_f feature_get;
    
    dnx_data_gtimer_ile_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ile_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ile_t;







typedef enum
{
    
    dnx_data_gtimer_ippa_is_supported,

    
    _dnx_data_gtimer_ippa_feature_nof
} dnx_data_gtimer_ippa_feature_e;



typedef int(
    *dnx_data_gtimer_ippa_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippa_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippa_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippa_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippa_feature_get_f feature_get;
    
    dnx_data_gtimer_ippa_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippa_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippa_t;







typedef enum
{
    
    dnx_data_gtimer_ippb_is_supported,

    
    _dnx_data_gtimer_ippb_feature_nof
} dnx_data_gtimer_ippb_feature_e;



typedef int(
    *dnx_data_gtimer_ippb_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippb_feature_get_f feature_get;
    
    dnx_data_gtimer_ippb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippb_t;







typedef enum
{
    
    dnx_data_gtimer_ippc_is_supported,

    
    _dnx_data_gtimer_ippc_feature_nof
} dnx_data_gtimer_ippc_feature_e;



typedef int(
    *dnx_data_gtimer_ippc_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippc_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippc_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippc_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippc_feature_get_f feature_get;
    
    dnx_data_gtimer_ippc_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippc_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippc_t;







typedef enum
{
    
    dnx_data_gtimer_ippd_is_supported,

    
    _dnx_data_gtimer_ippd_feature_nof
} dnx_data_gtimer_ippd_feature_e;



typedef int(
    *dnx_data_gtimer_ippd_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippd_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippd_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippd_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippd_feature_get_f feature_get;
    
    dnx_data_gtimer_ippd_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippd_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippd_t;







typedef enum
{
    
    dnx_data_gtimer_ippe_is_supported,

    
    _dnx_data_gtimer_ippe_feature_nof
} dnx_data_gtimer_ippe_feature_e;



typedef int(
    *dnx_data_gtimer_ippe_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippe_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippe_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippe_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippe_feature_get_f feature_get;
    
    dnx_data_gtimer_ippe_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippe_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippe_t;







typedef enum
{
    
    dnx_data_gtimer_ippf_is_supported,

    
    _dnx_data_gtimer_ippf_feature_nof
} dnx_data_gtimer_ippf_feature_e;



typedef int(
    *dnx_data_gtimer_ippf_feature_get_f) (
    int unit,
    dnx_data_gtimer_ippf_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ippf_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ippf_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ippf_feature_get_f feature_get;
    
    dnx_data_gtimer_ippf_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ippf_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ippf_t;







typedef enum
{
    
    dnx_data_gtimer_ips_is_supported,

    
    _dnx_data_gtimer_ips_feature_nof
} dnx_data_gtimer_ips_feature_e;



typedef int(
    *dnx_data_gtimer_ips_feature_get_f) (
    int unit,
    dnx_data_gtimer_ips_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ips_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ips_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ips_feature_get_f feature_get;
    
    dnx_data_gtimer_ips_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ips_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ips_t;







typedef enum
{
    
    dnx_data_gtimer_ipt_is_supported,

    
    _dnx_data_gtimer_ipt_feature_nof
} dnx_data_gtimer_ipt_feature_e;



typedef int(
    *dnx_data_gtimer_ipt_feature_get_f) (
    int unit,
    dnx_data_gtimer_ipt_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ipt_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ipt_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ipt_feature_get_f feature_get;
    
    dnx_data_gtimer_ipt_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ipt_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ipt_t;







typedef enum
{
    
    dnx_data_gtimer_iqm_is_supported,

    
    _dnx_data_gtimer_iqm_feature_nof
} dnx_data_gtimer_iqm_feature_e;



typedef int(
    *dnx_data_gtimer_iqm_feature_get_f) (
    int unit,
    dnx_data_gtimer_iqm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_iqm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_iqm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_iqm_feature_get_f feature_get;
    
    dnx_data_gtimer_iqm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_iqm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_iqm_t;







typedef enum
{
    
    dnx_data_gtimer_ire_is_supported,

    
    _dnx_data_gtimer_ire_feature_nof
} dnx_data_gtimer_ire_feature_e;



typedef int(
    *dnx_data_gtimer_ire_feature_get_f) (
    int unit,
    dnx_data_gtimer_ire_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ire_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ire_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ire_feature_get_f feature_get;
    
    dnx_data_gtimer_ire_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ire_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ire_t;







typedef enum
{
    
    dnx_data_gtimer_itpp_is_supported,

    
    _dnx_data_gtimer_itpp_feature_nof
} dnx_data_gtimer_itpp_feature_e;



typedef int(
    *dnx_data_gtimer_itpp_feature_get_f) (
    int unit,
    dnx_data_gtimer_itpp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_itpp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_itpp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_itpp_feature_get_f feature_get;
    
    dnx_data_gtimer_itpp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_itpp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_itpp_t;







typedef enum
{
    
    dnx_data_gtimer_itppd_is_supported,

    
    _dnx_data_gtimer_itppd_feature_nof
} dnx_data_gtimer_itppd_feature_e;



typedef int(
    *dnx_data_gtimer_itppd_feature_get_f) (
    int unit,
    dnx_data_gtimer_itppd_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_itppd_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_itppd_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_itppd_feature_get_f feature_get;
    
    dnx_data_gtimer_itppd_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_itppd_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_itppd_t;







typedef enum
{
    
    dnx_data_gtimer_kaps_is_supported,

    
    _dnx_data_gtimer_kaps_feature_nof
} dnx_data_gtimer_kaps_feature_e;



typedef int(
    *dnx_data_gtimer_kaps_feature_get_f) (
    int unit,
    dnx_data_gtimer_kaps_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_kaps_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_kaps_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_kaps_feature_get_f feature_get;
    
    dnx_data_gtimer_kaps_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_kaps_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_kaps_t;







typedef enum
{
    
    dnx_data_gtimer_mcp_is_supported,

    
    _dnx_data_gtimer_mcp_feature_nof
} dnx_data_gtimer_mcp_feature_e;



typedef int(
    *dnx_data_gtimer_mcp_feature_get_f) (
    int unit,
    dnx_data_gtimer_mcp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_mcp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_mcp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_mcp_feature_get_f feature_get;
    
    dnx_data_gtimer_mcp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_mcp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_mcp_t;







typedef enum
{
    
    dnx_data_gtimer_mrps_is_supported,

    
    _dnx_data_gtimer_mrps_feature_nof
} dnx_data_gtimer_mrps_feature_e;



typedef int(
    *dnx_data_gtimer_mrps_feature_get_f) (
    int unit,
    dnx_data_gtimer_mrps_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_mrps_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_mrps_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_mrps_feature_get_f feature_get;
    
    dnx_data_gtimer_mrps_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_mrps_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_mrps_t;







typedef enum
{
    
    dnx_data_gtimer_nmg_is_supported,

    
    _dnx_data_gtimer_nmg_feature_nof
} dnx_data_gtimer_nmg_feature_e;



typedef int(
    *dnx_data_gtimer_nmg_feature_get_f) (
    int unit,
    dnx_data_gtimer_nmg_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_nmg_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_nmg_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_nmg_feature_get_f feature_get;
    
    dnx_data_gtimer_nmg_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_nmg_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_nmg_t;







typedef enum
{
    
    dnx_data_gtimer_ocb_is_supported,

    
    _dnx_data_gtimer_ocb_feature_nof
} dnx_data_gtimer_ocb_feature_e;



typedef int(
    *dnx_data_gtimer_ocb_feature_get_f) (
    int unit,
    dnx_data_gtimer_ocb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ocb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ocb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ocb_feature_get_f feature_get;
    
    dnx_data_gtimer_ocb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ocb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ocb_t;







typedef enum
{
    
    dnx_data_gtimer_pdm_is_supported,

    
    _dnx_data_gtimer_pdm_feature_nof
} dnx_data_gtimer_pdm_feature_e;



typedef int(
    *dnx_data_gtimer_pdm_feature_get_f) (
    int unit,
    dnx_data_gtimer_pdm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_pdm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_pdm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_pdm_feature_get_f feature_get;
    
    dnx_data_gtimer_pdm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_pdm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_pdm_t;







typedef enum
{
    
    dnx_data_gtimer_pqp_is_supported,

    
    _dnx_data_gtimer_pqp_feature_nof
} dnx_data_gtimer_pqp_feature_e;



typedef int(
    *dnx_data_gtimer_pqp_feature_get_f) (
    int unit,
    dnx_data_gtimer_pqp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_pqp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_pqp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_pqp_feature_get_f feature_get;
    
    dnx_data_gtimer_pqp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_pqp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_pqp_t;







typedef enum
{
    
    dnx_data_gtimer_rqp_is_supported,

    
    _dnx_data_gtimer_rqp_feature_nof
} dnx_data_gtimer_rqp_feature_e;



typedef int(
    *dnx_data_gtimer_rqp_feature_get_f) (
    int unit,
    dnx_data_gtimer_rqp_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_rqp_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_rqp_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_rqp_feature_get_f feature_get;
    
    dnx_data_gtimer_rqp_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_rqp_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_rqp_t;







typedef enum
{
    
    dnx_data_gtimer_sch_is_supported,

    
    _dnx_data_gtimer_sch_feature_nof
} dnx_data_gtimer_sch_feature_e;



typedef int(
    *dnx_data_gtimer_sch_feature_get_f) (
    int unit,
    dnx_data_gtimer_sch_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_sch_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_sch_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_sch_feature_get_f feature_get;
    
    dnx_data_gtimer_sch_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_sch_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_sch_t;







typedef enum
{
    
    dnx_data_gtimer_sif_is_supported,

    
    _dnx_data_gtimer_sif_feature_nof
} dnx_data_gtimer_sif_feature_e;



typedef int(
    *dnx_data_gtimer_sif_feature_get_f) (
    int unit,
    dnx_data_gtimer_sif_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_sif_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_sif_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_sif_feature_get_f feature_get;
    
    dnx_data_gtimer_sif_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_sif_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_sif_t;







typedef enum
{
    
    dnx_data_gtimer_spb_is_supported,

    
    _dnx_data_gtimer_spb_feature_nof
} dnx_data_gtimer_spb_feature_e;



typedef int(
    *dnx_data_gtimer_spb_feature_get_f) (
    int unit,
    dnx_data_gtimer_spb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_spb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_spb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_spb_feature_get_f feature_get;
    
    dnx_data_gtimer_spb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_spb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_spb_t;







typedef enum
{
    
    dnx_data_gtimer_sqm_is_supported,

    
    _dnx_data_gtimer_sqm_feature_nof
} dnx_data_gtimer_sqm_feature_e;



typedef int(
    *dnx_data_gtimer_sqm_feature_get_f) (
    int unit,
    dnx_data_gtimer_sqm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_sqm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_sqm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_sqm_feature_get_f feature_get;
    
    dnx_data_gtimer_sqm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_sqm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_sqm_t;







typedef enum
{
    
    dnx_data_gtimer_tcam_is_supported,

    
    _dnx_data_gtimer_tcam_feature_nof
} dnx_data_gtimer_tcam_feature_e;



typedef int(
    *dnx_data_gtimer_tcam_feature_get_f) (
    int unit,
    dnx_data_gtimer_tcam_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_tcam_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_tcam_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_tcam_feature_get_f feature_get;
    
    dnx_data_gtimer_tcam_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_tcam_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_tcam_t;







typedef enum
{
    
    dnx_data_gtimer_tdu_is_supported,

    
    _dnx_data_gtimer_tdu_feature_nof
} dnx_data_gtimer_tdu_feature_e;



typedef int(
    *dnx_data_gtimer_tdu_feature_get_f) (
    int unit,
    dnx_data_gtimer_tdu_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_tdu_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_tdu_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_tdu_feature_get_f feature_get;
    
    dnx_data_gtimer_tdu_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_tdu_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_tdu_t;







typedef enum
{

    
    _dnx_data_gtimer_dcc_feature_nof
} dnx_data_gtimer_dcc_feature_e;



typedef int(
    *dnx_data_gtimer_dcc_feature_get_f) (
    int unit,
    dnx_data_gtimer_dcc_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_dcc_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_dcc_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_dcc_feature_get_f feature_get;
    
    dnx_data_gtimer_dcc_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_dcc_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_dcc_t;







typedef enum
{
    
    dnx_data_gtimer_cdb_is_supported,

    
    _dnx_data_gtimer_cdb_feature_nof
} dnx_data_gtimer_cdb_feature_e;



typedef int(
    *dnx_data_gtimer_cdb_feature_get_f) (
    int unit,
    dnx_data_gtimer_cdb_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cdb_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cdb_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cdb_feature_get_f feature_get;
    
    dnx_data_gtimer_cdb_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cdb_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cdb_t;







typedef enum
{
    
    dnx_data_gtimer_cdbm_is_supported,

    
    _dnx_data_gtimer_cdbm_feature_nof
} dnx_data_gtimer_cdbm_feature_e;



typedef int(
    *dnx_data_gtimer_cdbm_feature_get_f) (
    int unit,
    dnx_data_gtimer_cdbm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cdbm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cdbm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cdbm_feature_get_f feature_get;
    
    dnx_data_gtimer_cdbm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cdbm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cdbm_t;







typedef enum
{
    
    dnx_data_gtimer_cdpm_is_supported,

    
    _dnx_data_gtimer_cdpm_feature_nof
} dnx_data_gtimer_cdpm_feature_e;



typedef int(
    *dnx_data_gtimer_cdpm_feature_get_f) (
    int unit,
    dnx_data_gtimer_cdpm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_cdpm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_cdpm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_cdpm_feature_get_f feature_get;
    
    dnx_data_gtimer_cdpm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_cdpm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_cdpm_t;







typedef enum
{
    
    dnx_data_gtimer_ocbm_is_supported,

    
    _dnx_data_gtimer_ocbm_feature_nof
} dnx_data_gtimer_ocbm_feature_e;



typedef int(
    *dnx_data_gtimer_ocbm_feature_get_f) (
    int unit,
    dnx_data_gtimer_ocbm_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_ocbm_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_ocbm_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_ocbm_feature_get_f feature_get;
    
    dnx_data_gtimer_ocbm_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_ocbm_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_ocbm_t;







typedef enum
{
    
    dnx_data_gtimer_mss_is_supported,

    
    _dnx_data_gtimer_mss_feature_nof
} dnx_data_gtimer_mss_feature_e;



typedef int(
    *dnx_data_gtimer_mss_feature_get_f) (
    int unit,
    dnx_data_gtimer_mss_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_mss_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_mss_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_mss_feature_get_f feature_get;
    
    dnx_data_gtimer_mss_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_mss_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_mss_t;







typedef enum
{
    
    dnx_data_gtimer_msd_is_supported,

    
    _dnx_data_gtimer_msd_feature_nof
} dnx_data_gtimer_msd_feature_e;



typedef int(
    *dnx_data_gtimer_msd_feature_get_f) (
    int unit,
    dnx_data_gtimer_msd_feature_e feature);


typedef uint32(
    *dnx_data_gtimer_msd_block_index_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_gtimer_msd_block_index_max_value_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_gtimer_msd_feature_get_f feature_get;
    
    dnx_data_gtimer_msd_block_index_size_get_f block_index_size_get;
    
    dnx_data_gtimer_msd_block_index_max_value_get_f block_index_max_value_get;
} dnx_data_if_gtimer_msd_t;





typedef struct
{
    
    dnx_data_if_gtimer_rtp_t rtp;
    
    dnx_data_if_gtimer_fdt_t fdt;
    
    dnx_data_if_gtimer_fdtl_t fdtl;
    
    dnx_data_if_gtimer_pem_t pem;
    
    dnx_data_if_gtimer_dhc_t dhc;
    
    dnx_data_if_gtimer_cdu_t cdu;
    
    dnx_data_if_gtimer_esb_t esb;
    
    dnx_data_if_gtimer_cdum_t cdum;
    
    dnx_data_if_gtimer_mesh_topology_t mesh_topology;
    
    dnx_data_if_gtimer_bdm_t bdm;
    
    dnx_data_if_gtimer_hbc_t hbc;
    
    dnx_data_if_gtimer_ecgm_t ecgm;
    
    dnx_data_if_gtimer_cfc_t cfc;
    
    dnx_data_if_gtimer_cgm_t cgm;
    
    dnx_data_if_gtimer_crps_t crps;
    
    dnx_data_if_gtimer_ddha_t ddha;
    
    dnx_data_if_gtimer_ddp_t ddp;
    
    dnx_data_if_gtimer_dqm_t dqm;
    
    dnx_data_if_gtimer_edb_t edb;
    
    dnx_data_if_gtimer_epni_t epni;
    
    dnx_data_if_gtimer_epre_t epre;
    
    dnx_data_if_gtimer_eps_t eps;
    
    dnx_data_if_gtimer_erpp_t erpp;
    
    dnx_data_if_gtimer_etppa_t etppa;
    
    dnx_data_if_gtimer_etppb_t etppb;
    
    dnx_data_if_gtimer_etppc_t etppc;
    
    dnx_data_if_gtimer_fqp_t fqp;
    
    dnx_data_if_gtimer_ile_t ile;
    
    dnx_data_if_gtimer_ippa_t ippa;
    
    dnx_data_if_gtimer_ippb_t ippb;
    
    dnx_data_if_gtimer_ippc_t ippc;
    
    dnx_data_if_gtimer_ippd_t ippd;
    
    dnx_data_if_gtimer_ippe_t ippe;
    
    dnx_data_if_gtimer_ippf_t ippf;
    
    dnx_data_if_gtimer_ips_t ips;
    
    dnx_data_if_gtimer_ipt_t ipt;
    
    dnx_data_if_gtimer_iqm_t iqm;
    
    dnx_data_if_gtimer_ire_t ire;
    
    dnx_data_if_gtimer_itpp_t itpp;
    
    dnx_data_if_gtimer_itppd_t itppd;
    
    dnx_data_if_gtimer_kaps_t kaps;
    
    dnx_data_if_gtimer_mcp_t mcp;
    
    dnx_data_if_gtimer_mrps_t mrps;
    
    dnx_data_if_gtimer_nmg_t nmg;
    
    dnx_data_if_gtimer_ocb_t ocb;
    
    dnx_data_if_gtimer_pdm_t pdm;
    
    dnx_data_if_gtimer_pqp_t pqp;
    
    dnx_data_if_gtimer_rqp_t rqp;
    
    dnx_data_if_gtimer_sch_t sch;
    
    dnx_data_if_gtimer_sif_t sif;
    
    dnx_data_if_gtimer_spb_t spb;
    
    dnx_data_if_gtimer_sqm_t sqm;
    
    dnx_data_if_gtimer_tcam_t tcam;
    
    dnx_data_if_gtimer_tdu_t tdu;
    
    dnx_data_if_gtimer_dcc_t dcc;
    
    dnx_data_if_gtimer_cdb_t cdb;
    
    dnx_data_if_gtimer_cdbm_t cdbm;
    
    dnx_data_if_gtimer_cdpm_t cdpm;
    
    dnx_data_if_gtimer_ocbm_t ocbm;
    
    dnx_data_if_gtimer_mss_t mss;
    
    dnx_data_if_gtimer_msd_t msd;
} dnx_data_if_gtimer_t;




extern dnx_data_if_gtimer_t dnx_data_gtimer;


#endif 

