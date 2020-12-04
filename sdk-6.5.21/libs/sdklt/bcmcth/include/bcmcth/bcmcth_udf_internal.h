/*! \file bcmcth_udf_internal.h
 *
 * APIs, defines for CTH to interface with UDF LT sub-module.
 *
 * This file contains APIs, defines for CTH-UDF interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_UDF_INTERNAL_H
#define BCMCTH_UDF_INTERNAL_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * Index and base offset of HFE policy table for given udf anchor.
 */
typedef struct udf_policy_table_index_info_s {
    /*! HFE POLICY table index. */
    uint32_t index;

    /*! HFE POLICY table base offset. */
    uint32_t base_offset;
} udf_policy_table_index_info_t;

/*! Enum defines hfe table type for parsing stage. */
typedef enum udf_hfe_policy_table_e {
    /*! IPARSER1 HFE POLICY TABLE 1  */
    UDF_IPARSER1_HFE_POLICY_TABLE_1  = 0,

    /*! IPARSER1 HFE POLICY TABLE 3  */
    UDF_IPARSER1_HFE_POLICY_TABLE_3  = 1,

    /*! IPARSER1 HFE POLICY TABLE 2  */
    UDF_IPARSER1_HFE_POLICY_TABLE_2  = 2,

    /*! IPARSER1 HFE POLICY TABLE 0  */
    UDF_IPARSER1_HFE_POLICY_TABLE_0  = 3,

    /*! IPARSER1 HFE POLICY TABLE 5  */
    UDF_IPARSER1_HFE_POLICY_TABLE_5  = 4,

    /*! IPARSER1 HFE POLICY TABLE 4  */
    UDF_IPARSER1_HFE_POLICY_TABLE_4  = 5,

    /*! IPARSER0 HFE POLICY TABLE 0  */
    UDF_IPARSER0_HFE_POLICY_TABLE_0  = 6,

    /*! IPARSER2 HFE POLICY TABLE 4  */
    UDF_IPARSER2_HFE_POLICY_TABLE_4  = 7,

    /*! IPARSER2 HFE POLICY TABLE 3  */
    UDF_IPARSER2_HFE_POLICY_TABLE_3  = 8,

    /*! IPARSER2 HFE POLICY TABLE 0  */
    UDF_IPARSER2_HFE_POLICY_TABLE_0  = 9,

    /*! IPARSER2 HFE POLICY TABLE 1  */
    UDF_IPARSER2_HFE_POLICY_TABLE_1  = 10,

    /*! IPARSER2 HFE POLICY TABLE 2  */
    UDF_IPARSER2_HFE_POLICY_TABLE_2  = 11,

    /*! IPARSER HFE POLICY TABLE MAX */
    UDF_IPARSER_HFE_POLICY_TABLE_MAX = 12,
} udf_hfe_policy_table_t;

/*!
 * List of HFE POLICY table for given udf anchor.
 */
typedef struct udf_hfe_tbl_info_s {
    /*! HFE POLICY table type. */
    udf_hfe_policy_table_t tbl_type;

    /*! Parser. */
    uint8_t parser;

    /*! Number of entires need to be updated for each udf anchor. */
    uint32_t num_entries;

    /*! Policy table entry information. */
    const udf_policy_table_index_info_t *hfe_index_info;
} udf_hfe_tbl_info_t;

/*!
 * udf anchor
 */
typedef struct udf_anchor_s {
    /*! UDF header format. */
    int hdr;

    /*! Number of hfe policy tables. */
    uint32_t table_num;

    /*! Table info. */
    const udf_hfe_tbl_info_t **table;
} udf_anchor_t;

/*!
 * Information of a clock gating register related to a specific UDF container
 */
typedef struct udf_cont_clk_gating_reg_s {
    /*! SID of this register. */
    const bcmdrd_sid_t sid;

    /*! Bit-map in this register for the control of a specific UDF container. */
    const uint64_t     bits;
} udf_cont_clk_gating_reg_t;

/*!
 * Information of an UDF container
 */
typedef struct udf_cont_info_s {
    /*! Enum of this UDF container. */
    const int cont_id;

    /*! The number of clock gating register for this UDF container. */
    const int clk_gating_regs_cnt;

    /*! Clock gating information for this UDF container. */
    const udf_cont_clk_gating_reg_t *clk_gating_regs;
} udf_cont_info_t;

#endif /* BCMCTH_UDF_INTERNAL_H */
