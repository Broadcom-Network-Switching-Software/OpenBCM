/*! \file bcm56990_a0_ptm_uft.c
 *
 * Chip specific functions for PTM UFT
 *
 * This file contains the chip specific functions for PTM UFT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include "bcm56990_a0_ptm_uft.h"


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT

#define UFT_56990_EM_GRP_MAX       7
#define UFT_56990_PHY_BANKS_MAX    25

/*******************************************************************************
 * Typedefs
 */

/*! RM hash bank id */
typedef enum bank_id_e {
    /*! Not a valid bank choice. */
    BANK_INVALID = 0,

    /*! L2 dedicated bank 0. */
    L2_BANK0 = 1,

    /*! L2 dedicated bank 1. */
    L2_BANK1 = 2,

    /*! MPLS dedicated bank 0. */
    MPLS_BANK0 = 3,

    /*! MPLS dedicated bank 1. */
    MPLS_BANK1 = 4,

    /*! MPLS dedicated bank 0. */
    MPLS_BANK2 = 5,

    /*! MPLS dedicated bank 1. */
    MPLS_BANK3 = 6,

    /*! L3_TUNNEL dedicated bank 0. */
    L3_TUNNEL_BANK0 = 7,

    /*! L3_TUNNEL dedicated bank 1. */
    L3_TUNNEL_BANK1 = 8,

    /*! L3_TUNNEL dedicated bank 2. */
    L3_TUNNEL_BANK2 = 9,

    /*! L3_TUNNEL dedicated bank 3. */
    L3_TUNNEL_BANK3 = 10,

    /*! UFT shared bank 0 */
    UFT_BANK0 = 11,

    /*! UFT shared bank 1 */
    UFT_BANK1 = 12,

    /*! UFT shared bank 2 */
    UFT_BANK2 = 13,

    /*! UFT shared bank 3 */
    UFT_BANK3 = 14,

    /*! UFT shared bank 4 */
    UFT_BANK4 = 15,

    /*! UFT shared bank 5 */
    UFT_BANK5 = 16,

    /*! UFT shared bank 6 */
    UFT_BANK6 = 17,

    /*! UFT shared bank 7 */
    UFT_BANK7 = 18,

    /*! UFT mini bank 0 */
    UFT_MINI_BANK0 = 19,

    /*! UFT mini bank 1 */
    UFT_MINI_BANK1 = 20,

    /*! UFT mini bank 2 */
    UFT_MINI_BANK2 = 21,

    /*! UFT mini bank 3 */
    UFT_MINI_BANK3 = 22,

    /*! UFT mini bank 4 */
    UFT_MINI_BANK4 = 23,

    /*! UFT mini bank 5 */
    UFT_MINI_BANK5 = 24,

    /*! Max bank of each device. */
    BANK_MAX = 25
} bank_id_t;

/*! Device hash group id type. */
typedef enum group_id_e {
    /*! Not a valid group. */
    GROUP_INVALID = 0,

    /*! L2 features. */
    L2_HASH_GROUP = 1,

    /*! MPLS feature. */
    MPLS_HASH_GROUP = 2,

    /*! FP_EM feature. */
    FP_EM_HASH_GROUP = 3,

    /*! L3_TUNNEL feature. */
    L3_TUNNEL_HASH_GROUP = 4,

    /*! ALPM level 2 feature. */
    ALPM_LEVEL2_GROUP = 5,

    /*! ALPM level 3 feature. */
    ALPM_LEVEL3_GROUP = 6,
} group_id_t;

/*******************************************************************************
 * Private variables
 */

static uft_grp_info_t grps_info[UFT_56990_EM_GRP_MAX] = {
           {0},
           /* L2_HASH_GROUP */
           {.grp_id         = L2_HASH_GROUP,
            .ovly_ptsid     = L2_ENTRY_SINGLEm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 2,
            .fixed_banks[0] = L2_BANK0,
            .fixed_banks[1] = L2_BANK1,
            .lt_cnt         = 2,
            .lt_sids[0]     = L2_FDB_VLANt,
            .lt_sids[1]     = L2_FDB_VLAN_CCt,
           },
           /* MPLS_HASH_GROUP */
           {.grp_id         = MPLS_HASH_GROUP,
            .ovly_ptsid     = MPLS_ENTRY_DOUBLEm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 4,
            .fixed_banks[0] = MPLS_BANK0,
            .fixed_banks[1] = MPLS_BANK1,
            .fixed_banks[2] = MPLS_BANK2,
            .fixed_banks[3] = MPLS_BANK3,
            .lt_cnt         = 2,
            .lt_sids[0]     = TNL_MPLS_DECAPt,
            .lt_sids[1]     = TNL_MPLS_DECAP_TRUNKt,
           },
           /* FPEM_HASH_GROUP */
           {.grp_id         = FP_EM_HASH_GROUP,
            .ovly_ptsid     = EXACT_MATCH_4m,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 0,
            .lt_cnt         = 1,
            .lt_sids[0]     = FP_EM_ENTRYt,
           },
           /* L3_TUNNEL_GROUP */
           {.grp_id         = L3_TUNNEL_HASH_GROUP,
            .ovly_ptsid     = L3_TUNNEL_QUADm,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 4,
            .fixed_banks[0] = L3_TUNNEL_BANK0,
            .fixed_banks[1] = L3_TUNNEL_BANK1,
            .fixed_banks[2] = L3_TUNNEL_BANK2,
            .fixed_banks[3] = L3_TUNNEL_BANK3,
            .lt_cnt         = 2,
            .lt_sids[0]     = TNL_IPV4_DECAP_EMt,
            .lt_sids[1]     = TNL_IPV6_DECAP_EMt,
           },
           /* ALPM_LEVEL2_GROUP */
           {.grp_id         = ALPM_LEVEL2_GROUP,
            .ovly_ptsid     = L3_DEFIP_ALPM_LEVEL2m,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 0,
            .var_cnt        = 6,
            .var_banks[0]   = UFT_MINI_BANK0,
            .var_banks[1]   = UFT_MINI_BANK1,
            .var_banks[2]   = UFT_MINI_BANK2,
            .var_banks[3]   = UFT_MINI_BANK3,
            .var_banks[4]   = UFT_MINI_BANK4,
            .var_banks[5]   = UFT_MINI_BANK5,
            .lt_cnt         = 18,
            .lt_sids[0]     = L3_IPV4_UC_ROUTEt,
            .lt_sids[1]     = L3_IPV4_UC_ROUTE_OVERRIDEt,
            .lt_sids[2]     = L3_IPV4_UC_ROUTE_VRFt,
            .lt_sids[3]     = L3_IPV6_UC_ROUTEt,
            .lt_sids[4]     = L3_IPV6_UC_ROUTE_OVERRIDEt,
            .lt_sids[5]     = L3_IPV6_UC_ROUTE_VRFt,
            .lt_sids[6]     = L3_SRC_IPV4_UC_ROUTEt,
            .lt_sids[7]     = L3_SRC_IPV4_UC_ROUTE_OVERRIDEt,
            .lt_sids[8]     = L3_SRC_IPV4_UC_ROUTE_VRFt,
            .lt_sids[9]     = L3_SRC_IPV6_UC_ROUTEt,
            .lt_sids[10]    = L3_SRC_IPV6_UC_ROUTE_OVERRIDEt,
            .lt_sids[11]    = L3_SRC_IPV6_UC_ROUTE_VRFt,
            .lt_sids[12]    = L3_IPV4_MC_ROUTEt,
            .lt_sids[13]    = L3_IPV6_MC_ROUTEt,
            .lt_sids[14]    = L3_IPV4_COMP_DSTt,
            .lt_sids[15]    = L3_IPV4_COMP_SRCt,
            .lt_sids[16]    = L3_IPV6_COMP_DSTt,
            .lt_sids[17]    = L3_IPV6_COMP_SRCt,
            .is_alpm        = 1,
           },
           /* ALPM_LEVEL3_GROUP */
           {.grp_id         = ALPM_LEVEL3_GROUP,
            .ovly_ptsid     = L3_DEFIP_ALPM_LEVEL3m,
            .vector_type    = BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
            .fixed_cnt      = 0,
            .lt_cnt         = 18,
            .lt_sids[0]     = L3_IPV4_UC_ROUTEt,
            .lt_sids[1]     = L3_IPV4_UC_ROUTE_OVERRIDEt,
            .lt_sids[2]     = L3_IPV4_UC_ROUTE_VRFt,
            .lt_sids[3]     = L3_IPV6_UC_ROUTEt,
            .lt_sids[4]     = L3_IPV6_UC_ROUTE_OVERRIDEt,
            .lt_sids[5]     = L3_IPV6_UC_ROUTE_VRFt,
            .lt_sids[6]     = L3_SRC_IPV4_UC_ROUTEt,
            .lt_sids[7]     = L3_SRC_IPV4_UC_ROUTE_OVERRIDEt,
            .lt_sids[8]     = L3_SRC_IPV4_UC_ROUTE_VRFt,
            .lt_sids[9]     = L3_SRC_IPV6_UC_ROUTEt,
            .lt_sids[10]    = L3_SRC_IPV6_UC_ROUTE_OVERRIDEt,
            .lt_sids[11]    = L3_SRC_IPV6_UC_ROUTE_VRFt,
            .lt_sids[12]    = L3_IPV4_MC_ROUTEt,
            .lt_sids[13]    = L3_IPV6_MC_ROUTEt,
            .lt_sids[14]    = L3_IPV4_COMP_DSTt,
            .lt_sids[15]    = L3_IPV4_COMP_SRCt,
            .lt_sids[16]    = L3_IPV6_COMP_DSTt,
            .lt_sids[17]    = L3_IPV6_COMP_SRCt,
            .is_alpm        = 1,
           },
};

static uft_bank_info_t banks_info[UFT_56990_PHY_BANKS_MAX] = {
           {0},
           {.bank_id = L2_BANK0,
            .grp_cnt = 1,
            .grps[0] = L2_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L2_BANK1,
            .grp_cnt = 1,
            .grps[0] = L2_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = MPLS_BANK0,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = MPLS_BANK1,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = MPLS_BANK2,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = MPLS_BANK3,
            .grp_cnt = 1,
            .grps[0] = MPLS_HASH_GROUP,
            .base_entry_width = 105,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L3_TUNNEL_BANK0,
            .grp_cnt = 1,
            .grps[0] = L3_TUNNEL_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L3_TUNNEL_BANK1,
            .grp_cnt = 1,
            .grps[0] = L3_TUNNEL_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L3_TUNNEL_BANK2,
            .grp_cnt = 1,
            .grps[0] = L3_TUNNEL_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           {.bank_id = L3_TUNNEL_BANK3,
            .grp_cnt = 1,
            .grps[0] = L3_TUNNEL_HASH_GROUP,
            .base_entry_width = 96,
            .base_buket_width = 4,
            .num_base_buckets = 1024,
            .num_base_entries = 4096,
           },
           /* FP_EM can only use B0-B3 for HW restriction. */
           {.bank_id = UFT_BANK0,
            .grp_cnt = 2,
            .grps[0] = FP_EM_HASH_GROUP,
            .grps[1] = ALPM_LEVEL3_GROUP,
            .base_entry_width = 120,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK1,
            .grp_cnt = 2,
            .grps[0] = FP_EM_HASH_GROUP,
            .grps[1] = ALPM_LEVEL3_GROUP,
            .base_entry_width = 120,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK2,
            .grp_cnt = 2,
            .grps[0] = FP_EM_HASH_GROUP,
            .grps[1] = ALPM_LEVEL3_GROUP,
            .base_entry_width = 120,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK3,
            .grp_cnt = 2,
            .grps[0] = FP_EM_HASH_GROUP,
            .grps[1] = ALPM_LEVEL3_GROUP,
            .base_entry_width = 120,
            .base_buket_width = 4,
            .num_base_buckets = 8192,
            .num_base_entries = 32768,
           },
           {.bank_id = UFT_BANK4,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL3_GROUP,
           },
           {.bank_id = UFT_BANK5,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL3_GROUP,
           },
           {.bank_id = UFT_BANK6,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL3_GROUP,
           },
           {.bank_id = UFT_BANK7,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL3_GROUP,
           },
           {.bank_id = UFT_MINI_BANK0,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
           {.bank_id = UFT_MINI_BANK1,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
           {.bank_id = UFT_MINI_BANK2,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
           {.bank_id = UFT_MINI_BANK3,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
           {.bank_id = UFT_MINI_BANK4,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
           {.bank_id = UFT_MINI_BANK5,
            .grp_cnt = 1,
            .grps[0] = ALPM_LEVEL2_GROUP,
           },
};

static bcmptm_uft_bank_map_t grps_a_bmp_arr[UFT_56990_EM_GRP_MAX];

/*! Initial value for L2_ENTRY_HASH_CONTROL. */
static uft_hw_map_t l2_hash_ctrl = {
           .sid    = L2_ENTRY_HASH_CONTROLm,
           .index  = 0,
           .f_cnt  = 4,
           .f_ids  = { ROBUST_HASH_ENf,
                       HASH_TABLE_BANK_CONFIGf,
                       HASH_OFFSET_DEDICATED_BANK_0f,
                       HASH_OFFSET_DEDICATED_BANK_1f },
           .f_data = { 0, 3, 0, 32},
};

/*! Initial value for MPLS_ENTRY_HASH_CONTROL. */
static uft_hw_map_t mpls_hash_ctrl = {
           .sid    = MPLS_ENTRY_HASH_CONTROLm,
           .index  = 0,
           .f_cnt  = 6,
           .f_ids  = { ROBUST_HASH_ENf,
                       HASH_TABLE_BANK_CONFIGf,
                       HASH_OFFSET_DEDICATED_BANK_0f,
                       HASH_OFFSET_DEDICATED_BANK_1f,
                       HASH_OFFSET_DEDICATED_BANK_2f,
                       HASH_OFFSET_DEDICATED_BANK_3f},
           .f_data = { 0, 0xF, 0, 16, 32, 48 },
};
/*! Initial value for L3_TUNNEL_HASH_CONTROL. */
static uft_hw_map_t l3_tunnel_hash_ctrl = {
           .sid    = L3_TUNNEL_HASH_CONTROLm,
           .index  = 0,
           .f_cnt  = 6,
           .f_ids  = { ROBUST_HASH_ENf,
                       HASH_TABLE_BANK_CONFIGf,
                       HASH_OFFSET_DEDICATED_BANK_0f,
                       HASH_OFFSET_DEDICATED_BANK_1f,
                       HASH_OFFSET_DEDICATED_BANK_2f,
                       HASH_OFFSET_DEDICATED_BANK_3f },
           .f_data = { 0, 0xF, 0, 16, 32, 48 },

};
/*! Initial value for EXACT_MATCH_HASH_CONTROL. */
static uft_hw_map_t fp_em_hash_ctrl = {
           .sid    = EXACT_MATCH_HASH_CONTROLm,
           .index  = 0,
           .f_cnt  = 5,
           .f_ids  = { ROBUST_HASH_ENf,
                       LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
                       LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
                       LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
                       LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf },
           .f_data = { 0, 0, 0, 0, 0 },
};
/*! Initial value for UFT_SHARED_BANKS_CONTROL. */
static uft_hw_map_t uft_hash_ctrl = {
           .sid    = UFT_SHARED_BANKS_CONTROLm,
           .index  = 0,
           .f_cnt  = 8,
           .f_ids  = { B0_HASH_OFFSETf, B1_HASH_OFFSETf,
                       B2_HASH_OFFSETf, B3_HASH_OFFSETf,
                       B4_HASH_OFFSETf, B5_HASH_OFFSETf,
                       B6_HASH_OFFSETf, B7_HASH_OFFSETf },
           .f_data = { 6, 12, 18, 24,
                       0, 0, 0, 0 },
};
/*! Initial value for UFT_SHARED_BANKS_CONTROL. */
static uft_hw_map_t mini_uft_hash_ctrl = {
           .sid    = MINI_UFT_SHARED_BANKS_CONTROLm,
           .index  = 0,
           .f_cnt  = 6,
           .f_ids  = { B0_HASH_OFFSETf, B1_HASH_OFFSETf,
                       B2_HASH_OFFSETf, B3_HASH_OFFSETf,
                       B4_HASH_OFFSETf, B5_HASH_OFFSETf },
           .f_data = { 0, 0, 0, 0, 0, 0 },
};

/*! Initial value for L2_ENTRY_KEY_ATTRIBUTESm. */
static uft_hw_map_t l2_key_attr_0 = {
           .sid    = L2_ENTRY_KEY_ATTRIBUTESm,
           .index  = 0,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 15, 0, 0, 0 },
};
static uft_hw_map_t l2_key_attr_1 = {
           .sid    = L2_ENTRY_KEY_ATTRIBUTESm,
           .index  = 1,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 6, 0, 0, 0 },
};

/*! Initial value for MPLS_ENTRY_KEY_ATTRIBUTESm. */
static uft_hw_map_t mpls_key_attr_0 = {
           .sid    = MPLS_ENTRY_KEY_ATTRIBUTESm,
           .index  = 0,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 8, 0, 0, 0 },
};

/*! Initial value for L3_TUNNEL_KEY_ATTRIBUTESm. */
static uft_hw_map_t l3_tunnel_key_attr_0 = {
           .sid    = L3_TUNNEL_KEY_ATTRIBUTESm,
           .index  = 0,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 8, 0, 0, 0 },
};
static uft_hw_map_t l3_tunnel_key_attr_1 = {
           .sid    = L3_TUNNEL_KEY_ATTRIBUTESm,
           .index  = 1,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 18, 0, 0, 0 },
};
static uft_hw_map_t l3_tunnel_key_attr_2 = {
           .sid    = L3_TUNNEL_KEY_ATTRIBUTESm,
           .index  = 2,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 20, 2, 0, 0 },
};

/*! Initial value for EXACT_MATCH_KEY_ATTRIBUTESm. */
static uft_hw_map_t exact_match_key_attr_0 = {
           .sid    = EXACT_MATCH_KEY_ATTRIBUTESm,
           .index  = 0,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 3, 1, 0, 0 },
};
static uft_hw_map_t exact_match_key_attr_1 = {
           .sid    = EXACT_MATCH_KEY_ATTRIBUTESm,
           .index  = 1,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 11, 1, 0, 0 },
};
static uft_hw_map_t exact_match_key_attr_2 = {
           .sid    = EXACT_MATCH_KEY_ATTRIBUTESm,
           .index  = 2,
           .f_cnt  = 5,
           .f_ids  = { BUCKET_MODEf, KEY_WIDTHf, KEY_BASE_WIDTHf,
                       DATA_BASE_WIDTHf, HASH_LSB_OFFSETf },
           .f_data = { 0, 21, 2, 1, 0 },
};

/*******************************************************************************
 * Private Functions
 */

static int
dev_em_grp_fp_em_validate(int unit,
                          uft_dev_em_grp_t *grp,
                          uft_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);

    if (SHR_BITGET(grp->fbmp, DEVICE_EM_GROUPt_VECTOR_TYPEf)) {
        if (grp->vector_type != BCMPTM_RM_HASH_VEC_CRC32A_CRC32B) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    /* A hash table requires at least 2 banks. */
    if (grp->bank_cnt == 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

static int
dev_em_grp_alpm_level2_validate(int unit,
                                uft_dev_em_grp_t *grp,
                                uft_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);

    /* Check will be done in alpm. */

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_alpm_level3_validate(int unit,
                                uft_dev_em_grp_t *grp,
                                uft_dev_info_t *dev)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);

    /* Check will be done in alpm. */

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_fixed_grp_validate(int unit,
                              uft_dev_em_grp_t *grp,
                              uft_dev_info_t *dev)
{
    uint8_t type;

    SHR_FUNC_ENTER(unit);

    if (SHR_BITGET(grp->fbmp, DEVICE_EM_GROUPt_VECTOR_TYPEf)) {
        type = grp->vector_type;
        if (type != BCMPTM_RM_HASH_VEC_CRC32A_CRC32B) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    /* NUM_BANKSf and DEVICE_EM_BANK_IDf are ignored. */
    if (SHR_BITGET(grp->fbmp, DEVICE_EM_GROUPt_NUM_BANKSf)) {
        if (grp->bank_cnt != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_grp_fpem_set(int unit,
                    uft_dev_em_grp_t *grp,
                    uft_dev_info_t *dev,
                    uft_regs_info_t *regs)
{
    uint32_t bank_cnt;
    uint32_t new_cfg;
    uint32_t idx, f_cnt;
    bcmdrd_fid_t fpem_fids[4] = {
        LOGICAL_BANK_0_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf,
        LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf};

    SHR_FUNC_ENTER(unit);

    bank_cnt = grp->bank_cnt;
    new_cfg = (1 << bank_cnt) - 1;
    f_cnt = 0;
    regs->reg_map[0].sid = EXACT_MATCH_HASH_CONTROLm;
    regs->reg_map[0].f_ids[f_cnt] = HASH_TABLE_BANK_CONFIGf;
    regs->reg_map[0].f_data[f_cnt] = new_cfg;
    f_cnt++;
    regs->reg_map[0].f_ids[f_cnt] = ROBUST_HASH_ENf;
    regs->reg_map[0].f_data[f_cnt] = grp->robust;
    f_cnt++;


    for (idx = 0; idx < bank_cnt; idx++) {
        regs->reg_map[0].f_ids[f_cnt]  = fpem_fids[idx];
        regs->reg_map[0].f_data[f_cnt] = grp->bank[idx] - UFT_BANK0;
        f_cnt++;
    }
    regs->reg_map[0].f_cnt = f_cnt;
    regs->cnt = 1;

    SHR_FUNC_EXIT();
}

static int
dev_em_grp_fixed_grp_set(int unit,
                             uft_dev_em_grp_t *grp,
                             uft_dev_info_t *dev,
                             uft_regs_info_t *regs)

{
    uint32_t cnt = 0;
    bcmdrd_sid_t  sid;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(dev);

    if (SHR_BITGET(grp->fbmp, DEVICE_EM_GROUPt_ROBUSTf)) {
        switch (grp->grp_id) {
            case L2_HASH_GROUP:
                sid = L2_ENTRY_HASH_CONTROLm;
                break;
            case MPLS_HASH_GROUP:
                sid = MPLS_ENTRY_HASH_CONTROLm;
                break;
            case L3_TUNNEL_HASH_GROUP:
                sid = L3_TUNNEL_HASH_CONTROLm;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }

        regs->reg_map[0].sid = sid;
        regs->reg_map[0].f_ids[cnt] = ROBUST_HASH_ENf;
        regs->reg_map[0].f_data[cnt] = grp->robust;
        cnt++;
        regs->reg_map[0].f_cnt = cnt;
        regs->cnt = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
uft_bank_attrs_init(int unit, uft_dev_info_t *dev)
{
    uint32_t bank_id;

    SHR_FUNC_ENTER(unit);

    for (bank_id = BANK_INVALID + 1; bank_id < dev->max_banks; bank_id++) {
        switch (bank_id) {
            case L2_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case L2_BANK1:
                dev->bank_offset[bank_id] = 32;
                break;
            case MPLS_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case MPLS_BANK1:
                dev->bank_offset[bank_id] = 16;
                break;
            case MPLS_BANK2:
                dev->bank_offset[bank_id] = 32;
                break;
            case MPLS_BANK3:
                dev->bank_offset[bank_id] = 48;
                break;
            case L3_TUNNEL_BANK0:
                dev->bank_offset[bank_id] = 0;
                break;
            case L3_TUNNEL_BANK1:
                dev->bank_offset[bank_id] = 16;
                break;
            case L3_TUNNEL_BANK2:
                dev->bank_offset[bank_id] = 32;
                break;
            case L3_TUNNEL_BANK3:
                dev->bank_offset[bank_id] = 48;
                break;
            case UFT_BANK0:
                dev->bank_offset[bank_id] = 6;
                break;
            case UFT_BANK1:
                dev->bank_offset[bank_id] = 12;
                break;
            case UFT_BANK2:
                dev->bank_offset[bank_id] = 18;
                break;
            case UFT_BANK3:
                dev->bank_offset[bank_id] = 24;
                break;
            case UFT_BANK4:
            case UFT_BANK5:
            case UFT_BANK6:
            case UFT_BANK7:
            case UFT_MINI_BANK0:
            case UFT_MINI_BANK1:
            case UFT_MINI_BANK2:
            case UFT_MINI_BANK3:
            case UFT_MINI_BANK4:
            case UFT_MINI_BANK5:
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Level-1 registers
 *
 * \param [in] u Device u.
 * \param [in|out] regs Register info
 *
 * \return SHR_E_XXX.
 */
static int
bcm56990_a0_regs_init(int unit,
                      uft_dev_info_t *dev,
                      uft_regs_info_t *regs)
{
    int cnt = 0;
    SHR_FUNC_ENTER(unit);

    regs->reg_map[cnt++] = l2_hash_ctrl;
    regs->reg_map[cnt++] = mpls_hash_ctrl;
    regs->reg_map[cnt++] = l3_tunnel_hash_ctrl;
    regs->reg_map[cnt++] = fp_em_hash_ctrl;
    regs->reg_map[cnt++] = uft_hash_ctrl;
    regs->reg_map[cnt++] = mini_uft_hash_ctrl;
    regs->reg_map[cnt++] = l2_key_attr_0;
    regs->reg_map[cnt++] = l2_key_attr_1;
    regs->reg_map[cnt++] = mpls_key_attr_0;
    regs->reg_map[cnt++] = l3_tunnel_key_attr_0;
    regs->reg_map[cnt++] = l3_tunnel_key_attr_1;
    regs->reg_map[cnt++] = l3_tunnel_key_attr_2;
    regs->reg_map[cnt++] = exact_match_key_attr_0;
    regs->reg_map[cnt++] = exact_match_key_attr_1;
    regs->reg_map[cnt++] = exact_match_key_attr_2;
    regs->cnt = cnt;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize device specific info
 *
 * \param [in] u Device u.
 * \param [out] dev Device specific info.
 *
 * \return SHR_E_XXX.
 */
static int
bcm56990_a0_device_info_init(int unit,
                             const bcmptm_uft_var_drv_t *var_info,
                             uft_dev_info_t *dev)
{
    size_t size;
    uint32_t grp, bank, idx;

    SHR_FUNC_ENTER(unit);

    if (dev) {
        dev->max_grps = UFT_56990_EM_GRP_MAX;
        dev->max_banks = UFT_56990_PHY_BANKS_MAX;
        dev->grp_info = grps_info;
        dev->bank_info = banks_info;
        dev->grp_a_bmp_arr = grps_a_bmp_arr;
        for (grp = 1; grp < dev->max_grps; grp++) {
            for(bank = 1; bank < dev->max_banks; bank++) {
                for (idx = 0; idx < banks_info[bank].grp_cnt; idx++) {
                    if (grp == banks_info[bank].grps[idx]) {
                        SHR_BITSET(dev->grp_a_bmp_arr[grp].bm, bank);
                        break;
                    }
                }
            }
        }

        size = dev->max_grps * sizeof(bcmptm_uft_bank_map_t);
        SHR_ALLOC(dev->grp_c_bmp_arr, size, "bcmptmUftBankBmpCur");
        SHR_NULL_CHECK(dev->grp_c_bmp_arr, SHR_E_MEMORY);
        sal_memset(dev->grp_c_bmp_arr, 0, size);

        SHR_IF_ERR_EXIT(uft_bank_attrs_init(unit, dev));
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_uft_tbl_em_ctrl_validate(int unit,
                                     uft_tbl_em_ctrl_t *ctrl)
{
    SHR_FUNC_ENTER(unit);

    /* Check validity of move_depth. */
    if (SHR_BITGET(ctrl->fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf)) {
        if (ctrl->move_depth > BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
uft_bank_assigned_get(int unit, uft_dev_info_t *dev, int bank_id,
                      bool *assigned, uint32_t *r_grp_id)
{
    uint32_t i, grp_id;
    uft_grp_info_t *grp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dev, SHR_E_PARAM);
    SHR_NULL_CHECK(assigned, SHR_E_PARAM);
    *assigned = FALSE;
    for (i = 0; i < dev->max_grps; i++) {
        grp = &(dev->grp_info[i]);
        grp_id = grp->grp_id;
        if (SHR_BITGET(dev->grp_c_bmp_arr[grp_id].bm, bank_id)) {
            *assigned = TRUE;
            if (r_grp_id) {
                *r_grp_id = grp_id;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_uft_dev_em_grp_validate(int unit,
                                    uft_dev_em_grp_t *grp,
                                    uft_dev_info_t *dev)
{
    int idx, b_cnt = 0;
    uint32_t bank_id;
    bcmptm_uft_bank_map_t new_bmp;
    bool assigned = FALSE;
    uint32_t r_grp_id = 0;

    SHR_FUNC_ENTER(unit);

    switch (grp->grp_id) {
        case L2_HASH_GROUP:
        case MPLS_HASH_GROUP:
        case L3_TUNNEL_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fixed_grp_validate(unit, grp, dev));
            break;
        case FP_EM_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fp_em_validate(unit, grp, dev));
            break;
        case ALPM_LEVEL2_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_alpm_level2_validate(unit, grp, dev));
            break;
        case ALPM_LEVEL3_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_alpm_level3_validate(unit, grp, dev));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(new_bmp.bm, 0, sizeof(new_bmp));
    for (idx = 0; idx < grp->bank_cnt; idx++) {
        bank_id = grp->bank[idx];
        SHR_BITSET(new_bmp.bm, bank_id);
        if (SHR_BITGET(dev->grp_a_bmp_arr[grp->grp_id].bm, bank_id) == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (uft_bank_assigned_get(unit, dev, bank_id, &assigned, &r_grp_id));
        if (assigned && r_grp_id != grp->grp_id) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_BITCOUNT_RANGE(new_bmp.bm, b_cnt, 0, UFT_PHYS_BANKS_MAX);
    if (b_cnt != grp->bank_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_uft_dev_em_grp_set(int unit,
                               uft_dev_em_grp_t *grp,
                               uft_dev_info_t *dev,
                               uft_regs_info_t *regs)
{
    SHR_FUNC_ENTER(unit);

    switch (grp->grp_id) {
        case L2_HASH_GROUP:
        case MPLS_HASH_GROUP:
        case L3_TUNNEL_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fixed_grp_set(unit, grp, dev, regs));
            break;
        case FP_EM_HASH_GROUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fpem_set(unit, grp, dev, regs));
            break;
        case ALPM_LEVEL3_GROUP:
        case ALPM_LEVEL2_GROUP:
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_uft_dev_em_bank_validate(int unit, uft_dev_em_bank_t *bank)
{
    SHR_FUNC_ENTER(unit);

    switch (bank->bank_id) {
        /* These banks are reserved for ALPM level3 and level2 group. */
        case UFT_BANK4:
        case UFT_BANK5:
        case UFT_BANK6:
        case UFT_BANK7:
        case UFT_MINI_BANK0:
        case UFT_MINI_BANK1:
        case UFT_MINI_BANK2:
        case UFT_MINI_BANK3:
        case UFT_MINI_BANK4:
        case UFT_MINI_BANK5:
            SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_uft_dev_em_bank_set(int unit,
                                uft_dev_em_bank_t *bank,
                                uft_dev_info_t *dev,
                                uft_regs_info_t  *regs)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* DEVICE_EM_BANK LT has just one field. */
    if (!SHR_BITGET(bank->fbmp, DEVICE_EM_BANKt_FIELD_COUNT)) {
        SHR_EXIT();
    }

    switch (bank->bank_id) {
        /* Two L2 dedicated banks. */
        case L2_BANK0:
            sid = L2_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_0f;
            break;
        case L2_BANK1:
            sid = L2_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_1f;
            break;
        /* Four MPLS dedicated banks. */
        case MPLS_BANK0:
            sid = MPLS_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_0f;
            break;
        case MPLS_BANK1:
            sid = MPLS_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_1f;
            break;
        case MPLS_BANK2:
            sid = MPLS_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_2f;
            break;
        case MPLS_BANK3:
            sid = MPLS_ENTRY_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_3f;
            break;
        /* Four L3_TUNNEL dedicated banks. */
        case L3_TUNNEL_BANK0:
            sid = L3_TUNNEL_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_0f;
            break;
        case L3_TUNNEL_BANK1:
            sid = L3_TUNNEL_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_1f;
            break;
        case L3_TUNNEL_BANK2:
            sid = L3_TUNNEL_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_2f;
            break;
        case L3_TUNNEL_BANK3:
            sid = L3_TUNNEL_HASH_CONTROLm;
            fid = HASH_OFFSET_DEDICATED_BANK_3f;
            break;
        /* FP EM can only use B0-B3, so only their offsets are meaningful. */
        case UFT_BANK0:
            sid = UFT_SHARED_BANKS_CONTROLm;
            fid = B0_HASH_OFFSETf;
            break;
        case UFT_BANK1:
            sid = UFT_SHARED_BANKS_CONTROLm;
            fid = B1_HASH_OFFSETf;
            break;
        case UFT_BANK2:
            sid = UFT_SHARED_BANKS_CONTROLm;
            fid = B2_HASH_OFFSETf;
            break;
        case UFT_BANK3:
            sid = UFT_SHARED_BANKS_CONTROLm;
            fid = B3_HASH_OFFSETf;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

    dev->bank_offset[bank->bank_id] = bank->offset;

    regs->reg_map[0].sid = sid;
    regs->reg_map[0].f_ids[0] = fid;
    regs->reg_map[0].f_data[0] = bank->offset;
    regs->reg_map[0].f_cnt = 1;
    regs->cnt = 1;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */


int
bcm56990_a0_uft_driver_register(int unit, uft_driver_t *drv)
{
    SHR_FUNC_ENTER(unit);

    if (drv) {
        drv->regs_init            = bcm56990_a0_regs_init;
        drv->device_info_init     = bcm56990_a0_device_info_init;
        drv->tbl_em_ctrl_validate = bcm56990_a0_uft_tbl_em_ctrl_validate;
        drv->dev_em_grp_validate  = bcm56990_a0_uft_dev_em_grp_validate;
        drv->dev_em_grp_set       = bcm56990_a0_uft_dev_em_grp_set;
        drv->dev_em_bank_validate = bcm56990_a0_uft_dev_em_bank_validate;
        drv->dev_em_bank_set      = bcm56990_a0_uft_dev_em_bank_set;
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
