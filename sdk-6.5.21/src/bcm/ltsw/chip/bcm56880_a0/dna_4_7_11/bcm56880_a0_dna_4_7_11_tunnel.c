/*! \file bcm56880_a0_dna_4_7_11_tunnel.c
 *
 * BCM56880_A0 DNA_4_7_11 IP tunnel driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/mbcm/tunnel.h>
#include <bcm_int/ltsw/xfs/tunnel.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TUNNEL

/******************************************************************************
 * Private functions
 */
/* EGR_L3_TUNNEL_0 LT fields. */
static const bcmint_tunnel_fld_t
egr_l3_tunnel_0_flds[BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_EGR_L3_TUNNEL_IDX_0] = {
        EGR_L3_TUNNEL_IDX_0s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_EGR_L3_TUNNEL_IDX_0f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_EDIT_CTRL_ID] = {
        EDIT_CTRL_IDs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_EDIT_CTRL_IDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IPV6_SRC_ADDR] = {
        IPV6_SRC_ADDRs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_IPV6_SRC_ADDRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TOS_OR_EXP_REMARK_BASE_PTR] = {
        TOS_OR_EXP_REMARK_BASE_PTRs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_TOS_OR_EXP_REMARK_BASE_PTRf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TTL_OR_MPLS_CTRL] = {
        TTL_OR_MPLS_CTRLs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_TTL_OR_MPLS_CTRLf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_QOS_REMARK_CTRL] = {
        QOS_REMARK_CTRLs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_QOS_REMARK_CTRLf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TTL_VALUE] = {
        TTL_VALUEs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_TTL_VALUEf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IPV6_FLOW_LABEL_SEL] = {
        IPV6_FLOW_LABEL_SELs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_IPV6_FLOW_LABEL_SELf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IP_HDR_DF_CTRL] = {
        IP_HDR_DF_CTRLs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_IP_HDR_DF_CTRLf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TUNNEL_WORD_1] = {
        TUNNEL_WORD_1s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_TUNNEL_WORD_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_STRENGTH_PRFL_IDXf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/* EGR_L3_TUNNEL_1 LT fields. */
static const bcmint_tunnel_fld_t
egr_l3_tunnel_1_flds[BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_EGR_L3_TUNNEL_IDX_0] = {
        EGR_L3_TUNNEL_IDX_0s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_EGR_L3_TUNNEL_IDX_0f,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_1] = {
        IPV6_DST_ADDR_HI_1s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_IPV6_DST_ADDR_HI_1f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_0] = {
        IPV6_DST_ADDR_HI_0s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_IPV6_DST_ADDR_HI_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_LO] = {
        IPV6_DST_ADDR_LOs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_IPV6_DST_ADDR_LOf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_TUNNEL_WORD_0] = {
        TUNNEL_WORD_0s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_TUNNEL_WORD_0f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_TUNNEL_WORD_2] = {
        TUNNEL_WORD_2s,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_TUNNEL_WORD_2f,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_SEQ_NUM_PROFILE_IDX] = {
        SEQ_NUM_PROFILE_IDXs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_SEQ_NUM_PROFILE_IDXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_SEQ_NUM_COUNTER_IDX] = {
        SEQ_NUM_COUNTER_IDXs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_SEQ_NUM_COUNTER_IDXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_STRENGTH_PRFL_IDX] = {
        STRENGTH_PRFL_IDXs,
        BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_STRENGTH_PRFL_IDXf,
        false,
        false,
        0,
        NULL,
        NULL
    },
};

/* L3_IPV4_TUNNEL_TABLE LT fields. */
static const bcmint_tunnel_fld_t
l3_ipv4_tunnel_table_flds[BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_ENTRY_VALID] = {
        ENTRY_VALIDs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_ENTRY_VALIDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_SIP] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_SIP_MASK] = {
        IP_HDR_SIP_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_SIP_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_DIP] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_DIP_MASK] = {
        IP_HDR_DIP_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_DIP_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_PROTOCOL] = {
        IP_HDR_PROTOCOLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_PROTOCOL_MASK] = {
        IP_HDR_PROTOCOL_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_IP_HDR_PROTOCOL_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_SRC_PORT] = {
        L4_SRC_PORTs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L4_SRC_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_SRC_PORT_MASK] = {
        L4_SRC_PORT_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L4_SRC_PORT_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_DST_PORT] = {
        L4_DST_PORTs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L4_DST_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_DST_PORT_MASK] = {
        L4_DST_PORT_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L4_DST_PORT_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L3_IIF_STRENGTH] = {
        L3_IIF_STRENGTHs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_L3_IIF_STRENGTHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_USE_OUTER_HEADER_PHB] = {
        USE_OUTER_HEADER_PHBs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_USE_OUTER_HEADER_PHBf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_USE_OUTER_HDR_TTL] = {
        USE_OUTER_HDR_TTLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_USE_OUTER_HDR_TTLf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/* L3_IPV4_TUNNEL_EM LT fields. */
static const bcmint_tunnel_fld_t
l3_ipv4_tunnel_em_table_flds[BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_ENTRY_VALID] = {
        ENTRY_VALIDs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_ENTRY_VALIDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_IP_HDR_SIP] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_IP_HDR_DIP] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_L3_IIF_STRENGTH] = {
        L3_IIF_STRENGTHs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_L3_IIF_STRENGTHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_USE_OUTER_HEADER_PHB] = {
        USE_OUTER_HEADER_PHBs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_USE_OUTER_HEADER_PHBf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_USE_OUTER_HDR_TTL] = {
        USE_OUTER_HDR_TTLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_USE_OUTER_HDR_TTLf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/* L3_IPV6_TUNNEL_TABLE LT fields. */
static const bcmint_tunnel_fld_t
l3_ipv6_tunnel_table_flds[BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_ENTRY_VALID] = {
        ENTRY_VALIDs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_ENTRY_VALIDf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_ENTRY_PRIORITY] = {
        ENTRY_PRIORITYs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_ENTRY_PRIORITYf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_SIP] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_SIP_MASK] = {
        IP_HDR_SIP_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_SIP_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_DIP] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_DIP_MASK] = {
        IP_HDR_DIP_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_DIP_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_PROTOCOL] = {
        IP_HDR_PROTOCOLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_PROTOCOLf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_PROTOCOL_MASK] = {
        IP_HDR_PROTOCOL_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_IP_HDR_PROTOCOL_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_SRC_PORT] = {
        L4_SRC_PORTs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L4_SRC_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_SRC_PORT_MASK] = {
        L4_SRC_PORT_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L4_SRC_PORT_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_DST_PORT] = {
        L4_DST_PORTs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L4_DST_PORTf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_DST_PORT_MASK] = {
        L4_DST_PORT_MASKs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L4_DST_PORT_MASKf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L3_IIF_STRENGTH] = {
        L3_IIF_STRENGTHs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_L3_IIF_STRENGTHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_USE_OUTER_HEADER_PHB] = {
        USE_OUTER_HEADER_PHBs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_USE_OUTER_HEADER_PHBf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_USE_OUTER_HDR_TTL] = {
        USE_OUTER_HDR_TTLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_USE_OUTER_HDR_TTLf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

/* L3_IPV6_TUNNEL_EM LT fields. */
static const bcmint_tunnel_fld_t
l3_ipv6_tunnel_em_table_flds[BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_FLD_CNT] = {
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_ENTRY_VALID] = {
        ENTRY_VALIDs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_ENTRY_VALIDf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_IP_HDR_SIP] = {
        IP_HDR_SIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_IP_HDR_SIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_IP_HDR_DIP] = {
        IP_HDR_DIPs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_IP_HDR_DIPf,
        true,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_L3_IIF] = {
        L3_IIFs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_L3_IIFf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_L3_IIF_STRENGTH] = {
        L3_IIF_STRENGTHs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_L3_IIF_STRENGTHf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_USE_OUTER_HEADER_PHB] = {
        USE_OUTER_HEADER_PHBs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_USE_OUTER_HEADER_PHBf,
        false,
        false,
        0,
        NULL,
        NULL
    },
    [BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_USE_OUTER_HDR_TTL] = {
        USE_OUTER_HDR_TTLs,
        BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_USE_OUTER_HDR_TTLf,
        false,
        false,
        0,
        NULL,
        NULL
    }
};

static const bcmint_tunnel_tbl_t
bcm56880_a0_dna_4_7_11_tnl_tbl[BCMINT_LTSW_TNL_TBL_CNT] = {
    [BCMINT_LTSW_TNL_TBL_EGR_L3_TUNNEL_0] = {
        .name = EGR_L3_TUNNEL_0s,
        .tbl_id = BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_0t_FIELD_COUNT,
        .fld_bmp =
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_EGR_L3_TUNNEL_IDX_0) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_EDIT_CTRL_ID) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IPV6_SRC_ADDR) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TOS_OR_EXP_REMARK_BASE_PTR) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TTL_OR_MPLS_CTRL) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_QOS_REMARK_CTRL) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TTL_VALUE) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IPV6_FLOW_LABEL_SEL) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_IP_HDR_DF_CTRL) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_TUNNEL_WORD_1) |
            (1 << BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_0_STRENGTH_PRFL_IDX),
        .flds = egr_l3_tunnel_0_flds
    },
    [BCMINT_LTSW_TNL_TBL_EGR_L3_TUNNEL_1] = {
        .name = EGR_L3_TUNNEL_1s,
        .tbl_id = BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_EGR_L3_TUNNEL_1t_FIELD_COUNT,
        .fld_bmp =
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_EGR_L3_TUNNEL_IDX_0) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_1) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_HI_0) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_IPV6_DST_ADDR_LO) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_TUNNEL_WORD_0) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_TUNNEL_WORD_2) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_SEQ_NUM_PROFILE_IDX) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_SEQ_NUM_COUNTER_IDX) |
            (1<< BCMINT_LTSW_TNL_FLD_EGR_L3_TNL_1_STRENGTH_PRFL_IDX),
        .flds = egr_l3_tunnel_1_flds
    },
    [BCMINT_LTSW_TNL_TBL_L3_IPV4_TUNNEL_TABLE] = {
        .name = L3_IPV4_TUNNEL_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_TABLEt_FIELD_COUNT,
        .fld_bmp =
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_SIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_SIP_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_DIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_DIP_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_PROTOCOL) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_IP_HDR_PROTOCOL_MASK)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_SRC_PORT) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_SRC_PORT_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_DST_PORT) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L4_DST_PORT_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L3_IIF) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_USE_OUTER_HEADER_PHB)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_TABLE_USE_OUTER_HDR_TTL),
        .flds = l3_ipv4_tunnel_table_flds
    },
    [BCMINT_LTSW_TNL_TBL_L3_IPV4_TUNNEL_EM_TABLE] = {
        .name = L3_IPV4_TUNNEL_EM_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L3_IPV4_TUNNEL_EM_TABLEt_FIELD_COUNT,
        .fld_bmp =
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_IP_HDR_SIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_IP_HDR_DIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_L3_IIF) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_USE_OUTER_HEADER_PHB)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV4_TUNNEL_EM_TABLE_USE_OUTER_HDR_TTL),
        .flds = l3_ipv4_tunnel_em_table_flds
    },
    [BCMINT_LTSW_TNL_TBL_L3_IPV6_TUNNEL_TABLE] = {
        .name = L3_IPV6_TUNNEL_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_TABLEt_FIELD_COUNT,
        .fld_bmp =
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_SIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_SIP_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_DIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_DIP_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_PROTOCOL) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_IP_HDR_PROTOCOL_MASK)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_SRC_PORT) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_SRC_PORT_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_DST_PORT) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L4_DST_PORT_MASK) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L3_IIF) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_USE_OUTER_HEADER_PHB)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_TABLE_USE_OUTER_HDR_TTL),
        .flds = l3_ipv6_tunnel_table_flds
    },
    [BCMINT_LTSW_TNL_TBL_L3_IPV6_TUNNEL_EM_TABLE] = {
        .name = L3_IPV6_TUNNEL_EM_TABLEs,
        .tbl_id = BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt,
        .fld_cnt = BCM56880_A0_DNA_4_7_11_L3_IPV6_TUNNEL_EM_TABLEt_FIELD_COUNT,
        .fld_bmp =
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_IP_HDR_SIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_IP_HDR_DIP) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_L3_IIF) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_L3_IIF_STRENGTH) |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_USE_OUTER_HEADER_PHB)
            |
            (1 << BCMINT_LTSW_TNL_FLD_L3_IPV6_TUNNEL_EM_TABLE_USE_OUTER_HDR_TTL),
        .flds = l3_ipv6_tunnel_em_table_flds
    }
};

static int
bcm56880_a0_dna_4_7_11_tunnel_tbl_db_get(int unit,
                                         bcmint_tunnel_tbl_db_t *tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    /* Bitmap of tabless that valid on bcm56880 DNA_4_7_11. */
    tbl_db->tbl_bmp = (1 << BCMINT_LTSW_TNL_TBL_EGR_L3_TUNNEL_0) |
                      (1 << BCMINT_LTSW_TNL_TBL_EGR_L3_TUNNEL_1) |
                      (1 << BCMINT_LTSW_TNL_TBL_L3_IPV4_TUNNEL_TABLE) |
                      (1 << BCMINT_LTSW_TNL_TBL_L3_IPV4_TUNNEL_EM_TABLE) |
                      (1 << BCMINT_LTSW_TNL_TBL_L3_IPV6_TUNNEL_TABLE) |
                      (1 << BCMINT_LTSW_TNL_TBL_L3_IPV6_TUNNEL_EM_TABLE);

    tbl_db->tbls = bcm56880_a0_dna_4_7_11_tnl_tbl;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel driver function variable for bcm56880_a0 device.
 */
static mbcm_ltsw_tunnel_drv_t bcm56880_a0_dna_4_7_11_ltsw_tunnel_drv = {
     /* IP tunnel table database */
      .tunnel_tbl_db_get = bcm56880_a0_dna_4_7_11_tunnel_tbl_db_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_dna_4_7_11_ltsw_tunnel_drv_attach(int unit)
{
    return mbcm_ltsw_tunnel_drv_set(unit,
                                    &bcm56880_a0_dna_4_7_11_ltsw_tunnel_drv);
}

