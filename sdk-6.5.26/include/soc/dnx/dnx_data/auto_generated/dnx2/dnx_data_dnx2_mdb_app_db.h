
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DNX2_MDB_APP_DB_H_

#define _DNX_DATA_DNX2_MDB_APP_DB_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_max_dnx2_mdb_app_db.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_dnx2_mdb_app_db_init(
    int unit);







typedef enum
{

    
    _dnx_data_dnx2_mdb_app_db_fields_feature_nof
} dnx_data_dnx2_mdb_app_db_fields_feature_e;



typedef int(
    *dnx_data_dnx2_mdb_app_db_fields_feature_get_f) (
    int unit,
    dnx_data_dnx2_mdb_app_db_fields_feature_e feature);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1____get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1______valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1____get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1______valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1____get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1______valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1____get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1______valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1____get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1______valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48___valid_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49___valid_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dnx2_mdb_app_db_fields_feature_get_f feature_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ARP_AC_1TAG_STAT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BFD_IPV4_EXTRA_SIP___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_BASE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_ETH_RIF___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_COPY_INNER___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_GRE_GENEVE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_1_AH___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_1_AH___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_2_AH___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_2_3_AH___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_FHEI_Plus___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_MPLS_IFIT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG___valid_get_f numeric_mdb_field___ARR_INDEX___ETPP_ETPS_FORMATS___ETPS_SVTAG___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3_get_f numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3___valid_get_f numeric_mdb_field___ARR_INDEX___IRPP_IN_LIF_FORMATS___IN_LIF_IPvX_3___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___BFER_2ND_PASS_TERMINATION_WO_BFIR___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC_1TAG_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_AC___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___DUAL_HOMING___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_Custom___STAT_OBJECT_ID___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP_SA_tunneling___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ARP___ETPS_ARP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_BASE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_BIER___ETPS_BIER_NON_MPLS_INITIAL_4BYTES___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI_Plus___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_FHEI___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_DUMMY_TABLE_NOT_TO_USE___ETPS_MPLS_PHP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv2___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_ERSPAN___ETPS_ERSPANv3___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_IML___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_EVPN___ETPS_MPLS_1___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE_get_f numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_GTP___ETPS_GTPv1U___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPFIX_PSAMP___ETPS_IPFIX_PSAMP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_BFD_IPV4_EXTRA_SIP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_COPY_INNER___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_DOUBLE_UDP_VXLAN___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_GRE_GENEVE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_UDP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE_ECN___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL_VXLAN_GPE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV4_TUNNEL___ETPS_IPV4_TUNNEL___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GENEVE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_GRE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_RAW___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_Scale___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_UDP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN_ECN___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_IPV6_TUNNEL___ETPS_IPV6_TUNNEL_VXLAN___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE_get_f numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_L2TP___ETPS_L2TPv2___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE_get_f numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_LAWFUL_INTERCEPT___ETPS_DATA_VAR_SIZE_DATA___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE_get_f numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE___valid_get_f numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_IFIT___ETPS_MPLS_IFIT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1_TANDEM___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_1___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_1_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_2_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_3_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2_TANDEM___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_2___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_MPLS_TUNNEL___ETPS_MPLS_PHP_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE_get_f numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_NULL___ETPS_NULL___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE_get_f numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_OAM_REFLECTOR___ETPS_REFLECTOR_OAM___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION_NON_EM_BUSTER___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_STAT_PROTECTION___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_OUT_AC___ETPS_AC_TRIPLE_TAG___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE_get_f numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_PPPOE___ETPS_PPPOE___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE_get_f numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_IOAM_FL___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_AH___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_PWE___ETPS_MPLS_1___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE_get_f numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_RCH___ETPS_RCHv2___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE_get_f numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR_L2_MC___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE_get_f numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_REFLECTOR___ETPS_REFLECTOR___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE_get_f numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE_get_f numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_RIF_BASIC___ETPS_ETH_RIF___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_RSPAN___ETPS_RSPAN___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_first___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_EXT_second___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_DP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SFLOW___ETPS_SFLOW_HDR_SAMP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6_NO_SIDS___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_SRV6___ETPS_SRV6___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA_get_f numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA___valid_get_f numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___EEDB_LL_EXTRA_DATA___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE_get_f numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE___valid_get_f numeric_mdb_field___EEDB_TWAMP___ETPS_REFLECTOR_TWAMP___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_AC_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_DEFAULT_RESULT_TABLE___ETPS_NULL___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_DUAL_HOMING_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN_get_f numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___ESEM_DUAL_HOMING_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_DUAL_HOMING_PER_INTERFACE_NAMESPACE_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO_VLANS___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ETPS_L3_FODO___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_DB___ZERO_PADDING___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_DEVICE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___REMARK_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB___ETPS_L2_FODO___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_AC_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___ETPS_DUAL_HOMING___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___ESEM_PORT_BASED_SOURCE_ADDRESS_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE_get_f numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_PORT_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT_get_f numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT___valid_get_f numeric_mdb_field___ESEM_PORT_SVTAG___OUT_TM_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING_get_f numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING___valid_get_f numeric_mdb_field___ESEM_PORT_SVTAG___ZERO_PADDING___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE_get_f numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get_f numeric_mdb_field___ESEM_TUNNEL_SVTAG___ETPS_SVTAG___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1____get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1____get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1______valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC_1TAG_STAT___1______valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1____get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1____get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1______valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_EM_BUSTER___ETPS_ARP_AC___1______valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_SVTAG_ENABLE___ETPS_SVTAG___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1____get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1____get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1______valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC_1TAG_STAT___1______valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1____get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1____get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1______valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ARP_AC___1______valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1____get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1____get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1______valid_get_f numeric_mdb_field___ETPS_TYPE_ARR_INDEX___EEDB_TYPE_VSD_ENABLE___ETPS_ETH_RIF___1______valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM3_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM3_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___EXEM_IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PER_PORT_IGNORE_MAC___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PER_PORT_MY_MAC___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___EXEM_PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___GTP_CLASSIFICATION___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN___valid_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_0_VLANS___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN___valid_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_1_VLANS___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN___valid_get_f numeric_mdb_field___INNER_ETHERNET_VLAN_CLASSIFICATION_NETWORK_SCOPED_VLAN_2_VLANS___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_C_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_S_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_S_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_AC_TCAM_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get_f numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___IN_AC_UNTAGGED_DB___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_BR_E_C_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___IN_BR_E_S_VLAN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get_f numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___IN_BR_E_VLAN_DB___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___EGRESS_LLVP_PROFILE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___ESEM_NAME_SPACE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get_f numeric_mdb_field___JR1_MPLS_PUSH_COMMAND___ETPS_MPLS_1_STAT___RESULT_TYPE___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___L2TPV2_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___L2TPV3_DATA_MESSAGE_TT___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT_get_f numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___MAC_IN_MAC_LEARN_INFO___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT_get_f numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___MIM_TUNNEL_EM___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN_get_f numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN___valid_get_f numeric_mdb_field___MPLS_TERMINATION_SINGLE_LABEL_WITH_NETWORK_DOMAIN_DB___VLAN_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT_get_f numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT___valid_get_f numeric_mdb_field___PEM_PER_PORT_DB___KEY___IN_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_DTC_C_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_DTC_S_C_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_DTC_S_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_S_TAG_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_UNMATCHED_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get_f numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PON_UNTAGGED_CLASSIFICATION___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT_get_f numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PP_Port_SA_Based_Default___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT_get_f numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PP_Port_SA_Based_Tagged___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT_get_f numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT___valid_get_f numeric_mdb_field___PP_Port_SA_Based_VLAN_Specific___MAPPED_PP_PORT___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___TNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___TNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC_get_f numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC___valid_get_f numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONPTC___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING_get_f numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING___valid_get_f numeric_mdb_field___VIRT_TCAM___PRTTCAM_KEY_VP_PONZERO_PADDING___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___VNI2VRF___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get_f numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get_f numeric_mdb_field___VNI2VSI___NEXT_LAYER_NETWORK_DOMAIN___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0_get_f numeric_mdb_field___0_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___0___valid_get_f numeric_mdb_field___0___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1_get_f numeric_mdb_field___1_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___1___valid_get_f numeric_mdb_field___1___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2_get_f numeric_mdb_field___2_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___2___valid_get_f numeric_mdb_field___2___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3_get_f numeric_mdb_field___3_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___3___valid_get_f numeric_mdb_field___3___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4_get_f numeric_mdb_field___4_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___4___valid_get_f numeric_mdb_field___4___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5_get_f numeric_mdb_field___5_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___5___valid_get_f numeric_mdb_field___5___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6_get_f numeric_mdb_field___6_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___6___valid_get_f numeric_mdb_field___6___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7_get_f numeric_mdb_field___7_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___7___valid_get_f numeric_mdb_field___7___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8_get_f numeric_mdb_field___8_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___8___valid_get_f numeric_mdb_field___8___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9_get_f numeric_mdb_field___9_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___9___valid_get_f numeric_mdb_field___9___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10_get_f numeric_mdb_field___10_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___10___valid_get_f numeric_mdb_field___10___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11_get_f numeric_mdb_field___11_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___11___valid_get_f numeric_mdb_field___11___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12_get_f numeric_mdb_field___12_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___12___valid_get_f numeric_mdb_field___12___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13_get_f numeric_mdb_field___13_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___13___valid_get_f numeric_mdb_field___13___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14_get_f numeric_mdb_field___14_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___14___valid_get_f numeric_mdb_field___14___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15_get_f numeric_mdb_field___15_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___15___valid_get_f numeric_mdb_field___15___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16_get_f numeric_mdb_field___16_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___16___valid_get_f numeric_mdb_field___16___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17_get_f numeric_mdb_field___17_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___17___valid_get_f numeric_mdb_field___17___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18_get_f numeric_mdb_field___18_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___18___valid_get_f numeric_mdb_field___18___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19_get_f numeric_mdb_field___19_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___19___valid_get_f numeric_mdb_field___19___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20_get_f numeric_mdb_field___20_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___20___valid_get_f numeric_mdb_field___20___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21_get_f numeric_mdb_field___21_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___21___valid_get_f numeric_mdb_field___21___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22_get_f numeric_mdb_field___22_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___22___valid_get_f numeric_mdb_field___22___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23_get_f numeric_mdb_field___23_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___23___valid_get_f numeric_mdb_field___23___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24_get_f numeric_mdb_field___24_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___24___valid_get_f numeric_mdb_field___24___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25_get_f numeric_mdb_field___25_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___25___valid_get_f numeric_mdb_field___25___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26_get_f numeric_mdb_field___26_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___26___valid_get_f numeric_mdb_field___26___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27_get_f numeric_mdb_field___27_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___27___valid_get_f numeric_mdb_field___27___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28_get_f numeric_mdb_field___28_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___28___valid_get_f numeric_mdb_field___28___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29_get_f numeric_mdb_field___29_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___29___valid_get_f numeric_mdb_field___29___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30_get_f numeric_mdb_field___30_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___30___valid_get_f numeric_mdb_field___30___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31_get_f numeric_mdb_field___31_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___31___valid_get_f numeric_mdb_field___31___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32_get_f numeric_mdb_field___32_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___32___valid_get_f numeric_mdb_field___32___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33_get_f numeric_mdb_field___33_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___33___valid_get_f numeric_mdb_field___33___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34_get_f numeric_mdb_field___34_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___34___valid_get_f numeric_mdb_field___34___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35_get_f numeric_mdb_field___35_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___35___valid_get_f numeric_mdb_field___35___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36_get_f numeric_mdb_field___36_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___36___valid_get_f numeric_mdb_field___36___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37_get_f numeric_mdb_field___37_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___37___valid_get_f numeric_mdb_field___37___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38_get_f numeric_mdb_field___38_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___38___valid_get_f numeric_mdb_field___38___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39_get_f numeric_mdb_field___39_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___39___valid_get_f numeric_mdb_field___39___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40_get_f numeric_mdb_field___40_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___40___valid_get_f numeric_mdb_field___40___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41_get_f numeric_mdb_field___41_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___41___valid_get_f numeric_mdb_field___41___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42_get_f numeric_mdb_field___42_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___42___valid_get_f numeric_mdb_field___42___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43_get_f numeric_mdb_field___43_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___43___valid_get_f numeric_mdb_field___43___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44_get_f numeric_mdb_field___44_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___44___valid_get_f numeric_mdb_field___44___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45_get_f numeric_mdb_field___45_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___45___valid_get_f numeric_mdb_field___45___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46_get_f numeric_mdb_field___46_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___46___valid_get_f numeric_mdb_field___46___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47_get_f numeric_mdb_field___47_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___47___valid_get_f numeric_mdb_field___47___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48_get_f numeric_mdb_field___48_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___48___valid_get_f numeric_mdb_field___48___valid_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49_get_f numeric_mdb_field___49_get;
    
    dnx_data_dnx2_mdb_app_db_fields_numeric_mdb_field___49___valid_get_f numeric_mdb_field___49___valid_get;
} dnx_data_if_dnx2_mdb_app_db_fields_t;





typedef struct
{
    
    dnx_data_if_dnx2_mdb_app_db_fields_t fields;
} dnx_data_if_dnx2_mdb_app_db_t;




extern dnx_data_if_dnx2_mdb_app_db_t dnx_data_dnx2_mdb_app_db;


#endif 

