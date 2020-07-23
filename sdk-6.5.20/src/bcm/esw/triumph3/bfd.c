/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bfd.c
 * Purpose:    Bidirectional Forwarding Detection APIs.
 */

#include <soc/defs.h>

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_BFD)

#include <soc/hash.h>
#include <bcm_int/esw/bfd.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm/error.h>
#include <bcm/bfd.h>


/*
 * Function Vectors
 */
static bcm_esw_bfd_drv_t bcm_tr3_bfd_drv = {
    /* init                     */ bcmi_tr3_bfd_init,
    /* detach                   */ bcmi_xgs5_bfd_detach,
    /* endpoint_create          */ bcmi_xgs5_bfd_endpoint_create,
    /* endpoint_get             */ bcmi_xgs5_bfd_endpoint_get,
    /* endpoint_destroy         */ bcmi_xgs5_bfd_endpoint_destroy,
    /* endpoint_destroy_all     */ bcmi_xgs5_bfd_endpoint_destroy_all,
    /* endpoint_poll            */ bcmi_xgs5_bfd_endpoint_poll,
    /* event_register           */ bcmi_xgs5_bfd_event_register,
    /* event_unregister         */ bcmi_xgs5_bfd_event_unregister,
    /* endpoint_stat_get        */ bcmi_xgs5_bfd_endpoint_stat_get,
    /* auth_sha1_set            */ bcmi_xgs5_bfd_auth_sha1_set,
    /* auth_sha1_get            */ bcmi_xgs5_bfd_auth_sha1_get,
    /* auth_simple_password_set */ bcmi_xgs5_bfd_auth_simple_password_set,
    /* auth_simple_password_get */ bcmi_xgs5_bfd_auth_simple_password_get,
#ifdef BCM_WARM_BOOT_SUPPORT
    /* bfd_sync                 */ bcmi_xgs5_bfd_sync,
    /* _bfd_wb_downgrade_config_set */ _bcmi_bfd_wb_downgrade_config_set,
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    /* bfd_sw_dump              */ bcmi_xgs5_bfd_sw_dump,
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
    /* tx_start                 */ bcmi_xgs5_bfd_tx_start,
    /* tx_stop                  */ bcmi_xgs5_bfd_tx_stop,
    /* bfd_status_multi_get     */ bcmi_xgs5_bfd_status_multi_get,
    /* bfd_discard_stat_set     */ bcmi_xgs5_bfd_discard_stat_set,
    /* bfd_discard_stat_get     */ bcmi_xgs5_bfd_discard_stat_get,
    /* bcm_bfd_dump_trace       */ bcmi_xgs5_bfd_dump_trace,
};


/*
 * Device Specific HW Tables
 */

/* Device programming routines */
static bcm_xgs5_bfd_hw_calls_t bcm_tr3_bfd_hw_calls = {
    /* l3_tnl_term_entry_init   */ _bcm_tr3_l3_tnl_term_entry_init,
    /* mpls_lock                */ bcm_tr_mpls_lock,
    /* mpls_unlock              */ bcm_tr_mpls_unlock,
};

/* L2 Table */
static bcm_xgs5_bfd_l2_table_t bcm_tr3_bfd_l2_table = {
    /* mem                */ L2_ENTRY_1m,
    /* key_type           */ KEY_TYPEf,
    /* bfd_key_type       */ SOC_MEM_KEY_L2_ENTRY_1_BFD_BFD,
    /* valid              */ VALIDf,
    /* static             */ STATIC_BITf,
    /* session_id_type    */ BFD__SESSION_IDENTIFIER_TYPEf,
    /* your_discr         */ BFD__YOUR_DISCRIMINATORf,
    /* label              */ BFD__LABELf,
    /* session_index      */ BFD__RX_SESSION_IDXf,
    /* cpu_queue_class    */ INVALIDf,
    /* remote             */ BFD__REMOTEf,
    /* dst_module         */ BFD__MODULE_IDf,
    /* dst_port           */ BFD__PORT_NUMf,
    /* int_pri            */ BFD__INT_PRIf,
};

/* L3 IPv4 Unicast Table */
static bcm_xgs5_bfd_l3_ipv4_table_t bcm_tr3_bfd_l3_ipv4_table = {
    /* mem                */ L3_ENTRY_1m,
    /* vrf_id             */ IPV4UC__VRF_IDf, 
    /* ip_addr            */ IPV4UC__IP_ADDRf,
    /* key_type           */ KEY_TYPEf, 
    /* local_address      */ IPV4UC__LOCAL_ADDRESSf,
    /* bfd_enable         */ IPV4UC__BFD_ENABLEf,
};

/* L3 IPv6 Unicast Table */
static bcm_xgs5_bfd_l3_ipv6_table_t bcm_tr3_bfd_l3_ipv6_table = {
    /* mem                */ L3_ENTRY_2m,
    /* ip_addr_lwr_64     */ IPV6UC__IP_ADDR_LWR_64f,
    /* ip_addr_upr_64     */ IPV6UC__IP_ADDR_UPR_64f,
    /* key_type_0         */ KEY_TYPE_0f,
    /* key_type_1         */ KEY_TYPE_1f,
    /* vrf_id             */ IPV6UC__VRF_IDf,
    /* local_address      */ IPV6UC__LOCAL_ADDRESSf,
    /* bfd_enable         */ IPV6UC__BFD_ENABLEf,
    /* base_valid_0       */ INVALIDf,
    /* base_valid_1       */ INVALIDf,
    /* key_type           */ INVALIDf,
    /* data_type          */ INVALIDf,
};

/* L3 Tunnel Table */
static bcm_xgs5_bfd_l3_tunnel_table_t bcm_tr3_bfd_l3_tunnel_table = {
    /* mem                */ L3_TUNNELm,
    /* bfd_enable         */ IP__BFD_ENABLEf,
};

/* MPLS Table */
static bcm_xgs5_bfd_mpls_table_t bcm_tr3_bfd_mpls_table = {
    /* mem                */ MPLS_ENTRYm,
    /* valid              */ VALIDf,
    /* key_type           */ KEY_TYPEf,
    /* key_type_value     */ 16,
    /* mpls_label         */ MPLS__MPLS_LABELf,
    /* session_id_type    */ MPLS__SESSION_IDENTIFIER_TYPEf,
    /* bfd_enable         */ MPLS__BFD_ENABLEf,
    /* cw_check_ctrl      */ MPLS__CW_CHECK_CTRLf,
    /* pw_cc_type         */ MPLS__PW_CC_TYPEf,
    /* mpls_action_if_bos */ MPLS__MPLS_ACTION_IF_BOSf,
    /* l3_iif             */ MPLS__L3_IIFf,
    /* decap_use_ttl      */ MPLS__DECAP_USE_TTLf,
    /* mod id             */ MPLS__MODULE_IDf,
    /* Port num           */ MPLS__PORT_NUMf,
};

/* HW Definitions */
static bcm_xgs5_bfd_hw_defs_t    bcm_tr3_bfd_hw_defs[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *      bcmi_tr3_bfd_init
 * Purpose:
 *      Initialize the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_MEMORY Unable to allocate memory for internal control structures
 *      BCM_E_INTERNAL Failed to initialize
 * Notes:
 *      BFD initialization will return BCM_E_NONE for the following
 *      error conditions (i.e. feature not supported):
 *      - uKernel is not loaded
 *      - BFD application is not found in any of the uC uKernel
 */
int
bcmi_tr3_bfd_init(int unit)
{

    /* BFD feature enable bit set */
    uint8 bfd_feature_enable;
    uint32 val;

    /* HW Definition Tables */
    sal_memset(bcm_tr3_bfd_hw_defs, 0, sizeof(bcm_tr3_bfd_hw_defs));
    bcm_tr3_bfd_hw_defs[unit].hw_call   = &bcm_tr3_bfd_hw_calls;
    bcm_tr3_bfd_hw_defs[unit].l2        = &bcm_tr3_bfd_l2_table;
    bcm_tr3_bfd_hw_defs[unit].l3_ipv4   = &bcm_tr3_bfd_l3_ipv4_table;
    bcm_tr3_bfd_hw_defs[unit].l3_ipv6   = &bcm_tr3_bfd_l3_ipv6_table;
    bcm_tr3_bfd_hw_defs[unit].l3_tunnel = &bcm_tr3_bfd_l3_tunnel_table;
    bcm_tr3_bfd_hw_defs[unit].mpls      = &bcm_tr3_bfd_mpls_table;

    /*
     * BFD Feature Enable
     * lower bits will take precedence
     */
    bfd_feature_enable = soc_property_get(unit, spn_BFD_FEATURE_ENABLE, 1);

    if (bfd_feature_enable & BFD_MHOP_FEATURE_ENABLE) {
         /* Set UDP Dest port for MultiHop BFD session */
         BCM_IF_ERROR_RETURN(READ_BFD_RX_UDP_CONTROLr(unit,&val));
         soc_reg_field_set(unit, BFD_RX_UDP_CONTROLr, &val,
                           BFD_UDP_PORTM_HOPf, BFD_UDP_MULTI_HOP_DEST_PORT);
         BCM_IF_ERROR_RETURN(WRITE_BFD_RX_UDP_CONTROLr(unit, val));

         bcm_tr3_bfd_hw_defs[unit].bfd_feature_enable = BFD_MHOP_FEATURE_ENABLE;

    } else if (bfd_feature_enable &  BFD_MICRO_FEATURE_ENABLE) {
        /* Set UDP Dest port for Micro BFD session */
          BCM_IF_ERROR_RETURN(READ_BFD_RX_UDP_CONTROLr(unit,&val));
          soc_reg_field_set(unit, BFD_RX_UDP_CONTROLr, &val,
                            BFD_UDP_PORTM_HOPf, MICRO_BFD_DEST_PORT);
          BCM_IF_ERROR_RETURN(WRITE_BFD_RX_UDP_CONTROLr(unit, val));

          bcm_tr3_bfd_hw_defs[unit].bfd_feature_enable = BFD_MICRO_FEATURE_ENABLE;

    } else if (bfd_feature_enable & BFD_ECHO_FEATURE_ENABLE) {
          /* Set UDP Dest port for Echo BFD session */
          BCM_IF_ERROR_RETURN(READ_BFD_RX_UDP_CONTROLr(unit,&val));
          soc_reg_field_set(unit, BFD_RX_UDP_CONTROLr, &val,
                            BFD_UDP_PORTM_HOPf, BFD_ECHO_DEST_PORT);
          BCM_IF_ERROR_RETURN(WRITE_BFD_RX_UDP_CONTROLr(unit, val));

          bcm_tr3_bfd_hw_defs[unit].bfd_feature_enable = BFD_ECHO_FEATURE_ENABLE;
    } else if (bfd_feature_enable == 0) {
          /* Clear HW default values of BFD_RX_UDP_CONTROL reg */
          BCM_IF_ERROR_RETURN(READ_BFD_RX_UDP_CONTROLr(unit,&val));
          soc_reg_field_set(unit, BFD_RX_UDP_CONTROLr, &val,
                            BFD_UDP_PORTM_HOPf, 0);
          soc_reg_field_set(unit, BFD_RX_UDP_CONTROLr, &val,
                            BFD_UDP_PORT1_HOPf, 0);
          BCM_IF_ERROR_RETURN(WRITE_BFD_RX_UDP_CONTROLr(unit, val));
          bcm_tr3_bfd_hw_defs[unit].bfd_feature_enable = 0;
    }

    /* Initialize Common XGS5 BFD module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_bfd_init(unit, &bcm_tr3_bfd_drv, &bcm_tr3_bfd_hw_defs[unit]));

    return BCM_E_NONE;
}

#else   /* BCM_TRIUMPH3_SUPPORT && INCLUDE_BFD */
typedef int bcm_esw_tr3_bfd_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIUMPH3_SUPPORT && INCLUDE_BFD */

