/*! \file l3_intf_int.h
 *
 * L3 intf internal header file.
 * This file contains L3 interface definitions internal to the L3 intf module
 * itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_L3_INTF_INT_H
#define BCMI_LTSW_L3_INTF_INT_H

#include <bcm/l3.h>
#include <sal/sal_types.h>

#include <bcm_int/ltsw/types.h>

/* HA Sub component id for add_to_arl bitmap. */
#define BCMINT_LTSW_L3_INTF_HA_ADD2ARL (0)
#define BCMINT_LTSW_L3_INTF_HA_UL_OIF_INFO (1)


/* Do not modify DSCP. */
#define BCMINT_LTSW_L3_INTF_DSCP_ACTION_NONE (0)

/* Use DSCP value from L3_EIF table. */
#define BCMINT_LTSW_L3_INTF_DSCP_ACTION_FIXED (1)

/*
 * Use PHB_EGR_IP_INT_PRI_TO_DSCP_ID to get DSCP
 * value from PHB_EGR_IP_INT_PRI_TO_DSCP table.
 */
#define BCMINT_LTSW_L3_INTF_DSCP_ACTION_MAP (2)



/* Do not modify PRI and CFI. */
#define BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_NONE (0)

/*! Use PRI, CFI values from L3_EIF table. */
#define BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_FIXED (1)

/*
 * Use PHB_EGR_L2_INT_PRI_TO_OTAG_ID to get PRI, CFI
 * values from PHB_EGR_L2_INT_PRI_TO_OTAG table.
 */
#define BCMINT_LTSW_L3_INTF_L2_TAG_ACTION_MAP (2)


/* No tunnel. */
#define BCMINT_LTSW_L3_INTF_TNL_TYPE_NONE (0)

/* IPV4 tunnel. */
#define BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV4 (1)

/* IPV6 tunnel. */
#define BCMINT_LTSW_L3_INTF_TNL_TYPE_IPV6 (2)

/* MPLS tunnel. */
#define BCMINT_LTSW_L3_INTF_TNL_TYPE_MPLS (3)



/*
 * \brief Defines L3 egress interface information.
 *
 * This data structure defines the information in L3 egress interface.
 */
typedef struct bcmint_ltsw_l3_egr_intf_s {
    /* Create or replace. */
    uint32_t flags;

    /* Interface flags. */
    uint32_t intf_flags;

    /* Interface index. */
    int intf_id;

    /* MAC address for this interface. */
    bcm_mac_t mac_addr;

    /* VLAN ID for this interface. */
    bcm_vlan_t vid;

    /* Tunnel index. */
    int tunnel_idx;

    /* TTL threshold. */
    int ttl;

    /* MTU threshold. */
    int mtu;

    /* TTL decrement value. */
    int ttl_dec;

    /* Classification class ID. */
    int class_id;

    /* DSCP QoS setting. */
    bcm_l3_intf_qos_t  dscp_qos;

    /* VLAN QoS setting. */
    bcm_l3_intf_qos_t vlan_qos;

    /* Opaque ctrl ID. */
    int opaque_ctrl_id;

    /* MPLS flags. */
    uint32_t mpls_flags;

    /* MPLS VC Label. */
    bcm_mpls_label_t mpls_label;

    /* MPLS TTL threshold. */
    uint8 mpls_ttl;

    /* MPLS Exp. */
    uint8 mpls_exp;

    /* CPU interface. */
    bool cpu;
} bcmint_ltsw_l3_egr_intf_t;

/* Field description. */
typedef struct bcmint_ltsw_l3_intf_fld_s {
    /* Field name. */
    const char *fld_name;

    /* Field ID. */
    uint32_t fld_id;

    /* Key field. */
    bool key;

    /* Symbol field. */
    bool symbol;

    /* View type field. */
    bool view_t;

    /* Bitmap of valid views for field. Only used for multi-view LT. */
    uint32_t vt_bmp;

    /* Callback to get scalar value from symbol. */
    bcmi_ltsw_symbol_to_scalar_f symbol_to_scalar;

    /* Callback to get symbol from scalar value. */
    bcmi_ltsw_scalar_to_symbol_f scalar_to_symbol;

} bcmint_ltsw_l3_intf_fld_t;

/* The structure of L3 interface table. */
typedef struct bcmint_ltsw_l3_intf_tbl_s {
    /* Table name. */
    const char *name;

    /* Table ID. */
    uint32_t tbl_id;

    /* Table attributes. */
    uint32_t attr;

    /* Field count. */
    uint32_t fld_cnt;

    /* Bitmap of valid fields. */
    uint64_t fld_bmp;

    /* The fields of table. */
    const bcmint_ltsw_l3_intf_fld_t *flds;

} bcmint_ltsw_l3_intf_tbl_t;

/* L3 interface table database. */
typedef struct bcmint_ltsw_l3_intf_tbl_db_s {
    /* The table bitmap. */
    uint32_t tbl_bmp;

    /* The table array. */
    const bcmint_ltsw_l3_intf_tbl_t *tbls;
} bcmint_ltsw_l3_intf_tbl_db_t;

/*!
 * \brief Get L3 interface logical table info.
 *
 * \param [in] unit Unit Number.
 * \param [in] tbl_id L3 intf logical table description ID.
 * \param [out] tbl_info Pointer to table description info.
 *
 * \retval SHR_E_UNAVAIL Logical table is not supported.
 * \retval SHR_E_NONE No errer.
 */
extern int
bcmint_ltsw_l3_intf_tbl_get(int unit, int tbl_id,
                            const bcmint_ltsw_l3_intf_tbl_t **tbl_info);

#endif /* BCMI_LTSW_L3_INTF_INT_H */

