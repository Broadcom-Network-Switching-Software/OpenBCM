 /* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       subport.c
 * Purpose:    Subport CoE related functions.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/subport.h>

#include <bcm_int/esw/subport.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trident3.h>

/*
 * Function Vector
 */
static bcm_esw_subport_drv_t bcm_td3_subport_drv = {

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    /* subport_init                */ bcm_tr2_subport_init,
    /* subport_group_create        */ bcm_tr2_subport_group_create,
    /* subport_group_get           */ bcm_tr2_subport_group_get,
    /* subport_group_traverse      */ NULL,
    /* subport_group_destroy       */ bcm_tr2_subport_group_destroy,
    /* subport_linkphy_config_set  */ NULL,
    /* subport_linkphy_config_get  */ NULL,
    /* subport_port_add            */ bcm_tr2_subport_port_add,
    /* subport_port_get            */ bcm_tr2_subport_port_get,
    /* subport_port_traverse       */ bcm_tr2_subport_port_traverse,
    /* subport_port_stat_set       */ NULL,
    /* subport_port_stat_get       */ NULL,
    /* subport_port_delete         */ bcm_tr2_subport_port_delete,
    /* subport_cleanup             */ bcm_tr2_subport_cleanup,
#else
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
#endif

    /* coe_init                */ bcm_td3_subport_coe_init,
    /* coe_group_create        */ bcmi_xgs5_subport_coe_group_create,
    /* coe_group_get           */ bcmi_xgs5_subport_coe_group_get,
    /* coe_group_traverse      */ bcmi_xgs5_subport_coe_group_traverse,
    /* coe_group_destroy       */ bcmi_xgs5_subport_coe_group_destroy,
    /* coe_linkphy_config_set  */ NULL,
    /* coe_linkphy_config_get  */ NULL,
    /* coe_port_add            */ bcmi_xgs5_subport_coe_port_add,
    /* coe_port_get            */ bcmi_xgs5_subport_coe_port_get,
    /* coe_port_traverse       */ bcmi_xgs5_subport_coe_port_traverse,
    /* coe_port_stat_set       */ NULL,
    /* coe_port_stat_get       */ NULL,
    /* coe_port_delete         */ bcmi_xgs5_subport_coe_port_delete,
    /* coe_cleanup             */ bcmi_xgs5_subport_coe_cleanup,
    /* subport_linkphy_rx_error_register */ NULL,
    /* subport_linkphy_rx_error_unregister */ NULL,
    /* subport_gport_modport_get */ bcmi_xgs5_subport_gport_modport_get
};

/*
 * Device Specific HW Tables
 */

/* ING Port Table */
static bcmi_xgs5_subport_coe_ing_port_table_t bcmi_td3_subport_coe_ing_port_table = {
    /* mem */ ING_DEVICE_PORTm,
    /* port_type */ PORT_TYPEf
};

/* EGR Port Table */
static bcmi_xgs5_subport_coe_egr_port_table_t  bcmi_td3_subport_coe_egr_port_table = {
    /* mem */ EGR_PORTm,
    /* port_type */ PORT_TYPEf
};

/* Subport Tag SGPP Memory Table */
static bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t bcmi_td3_subport_coe_subport_tag_sgpp_table = {
    /* mem */                      SUBPORT_ID_TO_SGPP_MAPm,
    /* valid */                      BASE_VALIDf,
    /* subport_tag */                SUBPORT_IDf,
    /* subport_tag_mask */         INVALIDf,
    /* subport_tag_namespace */     SUBPORT_ID_NAMESPACEf,
    /* subport_tag_namespace_mask */ INVALIDf,
    /* src_modid */                  INVALIDf, /* src port and modid combined in SGPP */
    /* src_port */                 SGPPf,
    /* phb_enable */                 PHB_ENABLEf,
    /* int_pri */                 INT_PRIf,
    /* cng */                     CNGf
};

/* Modport map subport Memory Table */
bcmi_xgs5_subport_coe_modport_map_subport_table_t bcmi_td3_subport_coe_modport_map_subport_table = {
    /* mem */        MODPORT_MAP_SUBPORTm,
    /* dest */     DESTf,
    /* is_trunk */ ISTRUNKf,
    /* enable */ ENABLEf,
#ifdef BCM_FIREBOLT6_SUPPORT
    /* DST_SUBPORT_NUM */ DST_SUBPORT_NUMf,
#endif
};

/* Egress subport tag dot1p Table */
static bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t bcmi_td3_subport_coe_egr_subport_tag_dot1p_table = {
    /* mem */                   EGR_ZONE_3_DOT1P_MAPPING_TABLE_1m,
    /* subport_tag_priority */  PRIf,
    /* subport_tag_cfi */       CFIf
};

/* HW Definitions */
static bcmi_xgs5_subport_coe_hw_defs_t    bcmi_td3_subport_coe_hw_defs;

int bcm_td3_subport_coe_init(int unit)
{
    /* HW Definition Tables */
    sal_memset(&bcmi_td3_subport_coe_hw_defs, 0, sizeof(bcmi_td3_subport_coe_hw_defs));
    bcmi_td3_subport_coe_hw_defs.igr_port =
        &bcmi_td3_subport_coe_ing_port_table;
    bcmi_td3_subport_coe_hw_defs.egr_port   =
        &bcmi_td3_subport_coe_egr_port_table;
    bcmi_td3_subport_coe_hw_defs.subport_tag_sgpp =
        &bcmi_td3_subport_coe_subport_tag_sgpp_table;
    bcmi_td3_subport_coe_hw_defs.modport_map_subport =
        &bcmi_td3_subport_coe_modport_map_subport_table;
    bcmi_td3_subport_coe_hw_defs.egr_subport_tag_dot1p =
        &bcmi_td3_subport_coe_egr_subport_tag_dot1p_table;

    /* Initialize Common XGS5 CoE module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_subport_init(unit, &bcm_td3_subport_drv, &bcmi_td3_subport_coe_hw_defs));
    return 0;
}
