/*! \file bcm56990_a0_l3_fib.c
 *
 * BCM56990_A0 L3 FIB subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/mbcm/l3_aacl.h>
#include <bcm_int/ltsw/xgs/l3_aacl.h>
#include <bcm_int/ltsw/l3_aacl_int.h>
#include <bcm_int/ltsw/property.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/*
 * L3_IPV4_COMP_DST
 */
static const
bcmint_ltsw_l3_aacl_fld_t l3_aacl_ipv4_dst_flds[BCMINT_XGS_L3_AACL_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = {
        IPV4s,
        L3_IPV4_COMP_DSTt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_IPV4_COMP_DSTt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_MAC] = {
        MACs,
        L3_IPV4_COMP_DSTt_MACf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_VRF] = {
        VRFs,
        L3_IPV4_COMP_DSTt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_L4_PORT] = {
        L4_PORTs,
        L3_IPV4_COMP_DSTt_L4_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL] = {
        IP_PROTOCOLs,
        L3_IPV4_COMP_DSTt_IP_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS] = {
        TCP_FLAGSs,
        L3_IPV4_COMP_DSTt_TCP_FLAGSf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_COMP_DSTt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_HIT] = {
        HITs,
        L3_IPV4_COMP_DSTt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV4_COMP_SRC
 */
static const
bcmint_ltsw_l3_aacl_fld_t l3_aacl_ipv4_src_flds[BCMINT_XGS_L3_AACL_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = {
        IPV4s,
        L3_IPV4_COMP_SRCt_IPV4f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = {
        IPV4_MASKs,
        L3_IPV4_COMP_SRCt_IPV4_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_MAC] = {
        MACs,
        L3_IPV4_COMP_SRCt_MACf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_VRF] = {
        VRFs,
        L3_IPV4_COMP_SRCt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_L4_PORT] = {
        L4_PORTs,
        L3_IPV4_COMP_SRCt_L4_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL] = {
        IP_PROTOCOLs,
        L3_IPV4_COMP_SRCt_IP_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS] = {
        TCP_FLAGSs,
        L3_IPV4_COMP_SRCt_TCP_FLAGSf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV4_COMP_SRCt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_HIT] = {
        HITs,
        L3_IPV4_COMP_SRCt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_COMP_DST
 */
static const
bcmint_ltsw_l3_aacl_fld_t l3_aacl_ipv6_dst_flds[BCMINT_XGS_L3_AACL_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_IPV6_COMP_DSTt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_IPV6_COMP_DSTt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_IPV6_COMP_DSTt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_IPV6_COMP_DSTt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_VRF] = {
        VRFs,
        L3_IPV6_COMP_DSTt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_L4_PORT] = {
        L4_PORTs,
        L3_IPV6_COMP_DSTt_L4_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL] = {
        IP_PROTOCOLs,
        L3_IPV6_COMP_DSTt_IP_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS] = {
        TCP_FLAGSs,
        L3_IPV6_COMP_DSTt_TCP_FLAGSf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_COMP_DSTt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_HIT] = {
        HITs,
        L3_IPV6_COMP_DSTt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/*
 * L3_IPV6_COMP_SRC
 */
static const
bcmint_ltsw_l3_aacl_fld_t l3_aacl_ipv6_src_flds[BCMINT_XGS_L3_AACL_TBL_FLD_CNT] = {
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0] = {
        IPV6_LOWERs,
        L3_IPV6_COMP_SRCt_IPV6u_LOWERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1] = {
        IPV6_UPPERs,
        L3_IPV6_COMP_SRCt_IPV6u_UPPERf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK] = {
        IPV6_LOWER_MASKs,
        L3_IPV6_COMP_SRCt_IPV6u_LOWER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1_MASK] = {
        IPV6_UPPER_MASKs,
        L3_IPV6_COMP_SRCt_IPV6u_UPPER_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_VRF] = {
        VRFs,
        L3_IPV6_COMP_SRCt_VRFf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_L4_PORT] = {
        L4_PORTs,
        L3_IPV6_COMP_SRCt_L4_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL] = {
        IP_PROTOCOLs,
        L3_IPV6_COMP_SRCt_IP_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS] = {
        TCP_FLAGSs,
        L3_IPV6_COMP_SRCt_TCP_FLAGSf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_CLASS_ID] = {
        CLASS_IDs,
        L3_IPV6_COMP_SRCt_CLASS_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_XGS_L3_AACL_TBL_HIT] = {
        HITs,
        L3_IPV6_COMP_SRCt_HITf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

static const bcmint_ltsw_l3_aacl_tbl_t l3_aacl_tbls[] = {
    [BCMINT_LTSW_L3_AACL_TBL_IPV4_DST] = {
        .name = L3_IPV4_COMP_DSTs,
        .tbl_id = L3_IPV4_COMP_DSTt,
        .fld_cnt = L3_IPV4_COMP_DSTt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_MAC) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_VRF) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_L4_PORT) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_HIT),
        .flds = l3_aacl_ipv4_dst_flds
    },
    [BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC] = {
        .name = L3_IPV6_MC_ROUTEs,
        .tbl_id = L3_IPV6_MC_ROUTEt,
        .fld_bmp = (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_MAC) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_VRF) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_L4_PORT) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_HIT),
        .flds = l3_aacl_ipv4_src_flds
    },
    [BCMINT_LTSW_L3_AACL_TBL_IPV6_DST] = {
        .name = L3_IPV4_COMP_DSTs,
        .tbl_id = L3_IPV4_COMP_DSTt,
        .fld_cnt = L3_IPV4_COMP_DSTt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_VRF) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_L4_PORT) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_HIT),
        .flds = l3_aacl_ipv6_dst_flds
    },
    [BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC] = {
        .name = L3_IPV4_COMP_DSTs,
        .tbl_id = L3_IPV4_COMP_DSTt,
        .fld_cnt = L3_IPV4_COMP_DSTt_FIELD_COUNT,
        .fld_bmp = (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_0_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_ADDR_1_MASK) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_VRF) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_L4_PORT) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_IP_PROTOCOL) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_TCP_FLAGS) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_CLASS_ID) |
                   (1 << BCMINT_XGS_L3_AACL_TBL_HIT),
        .flds = l3_aacl_ipv6_src_flds
    },
};

int
bcm56990_a0_ltsw_l3_aacl_tbl_db_get(
    int unit,
    bcmint_ltsw_l3_aacl_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_L3_AACL_TBL_IPV4_DST) |
                      (1 << BCMINT_LTSW_L3_AACL_TBL_IPV6_DST) |
                      (1 << BCMINT_LTSW_L3_AACL_TBL_IPV4_SRC) |
                      (1 << BCMINT_LTSW_L3_AACL_TBL_IPV6_SRC);

    tbl_db->tbls = l3_aacl_tbls;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief L3_fib sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_l3_aacl_drv_t bcm56990_a0_l3_aacl_sub_drv = {
    .l3_aacl_tbl_db_get = bcm56990_a0_ltsw_l3_aacl_tbl_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_l3_aacl_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_aacl_drv_set(unit, &bcm56990_a0_l3_aacl_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
