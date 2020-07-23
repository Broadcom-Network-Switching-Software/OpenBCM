/*! \file bcm56780_a0_pkt_trace_internal.c
 *
 * Packet trace internal function
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmpkt/bcmpkt_trace_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>

#define MAX_ING_DOP_FORMATS(_u_)     204
#define MAX_EGR_DOP_FORMATS(_u_)     101
#define BASE_DOP_ID                  01
#define MAX_ING_DOP_READ_COUNT       200
#define DOP_INGRESS                  0x2
#define DOP_EGRESS                   0x4
#define NUM_PP_PIPES                 0x2

#ifndef WORDS2BYTES
#define WORDS2BYTES(x) ((x) * 4)
#endif

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_TRACE

typedef enum dop_ctrl_cmd_e {
    DOP_CLEAR = 0,
    DOP_COLLECT = 1,
    DOP_SELF_TRIGGER = 2,
    DOP_INIT = 3,
} dop_ctrl_cmd_t;

/*
 * Array of structure entries mapping global DOP number to
 * chip spcific DOP ID and Group ID.
 */
extern bcmpkt_trace_dop_id_map_t bcm56780_a0_trace_dop_id_map;

/*
 * The array maintains the mapping between PT ID to DOP ID, DOP Group and
 * DOP field ID. This mapping is static and independent of NPL.
 */
const bcmpkt_trace_pt_to_dop_map_t bcm56780_a0_pt_to_dop_map[] = {
    {
        .pt_name = "MEMDB_EFTA10_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA10_I1T_00_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA10_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA10_I1T_01_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA10_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA10_MEM2m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA10_I1T_02_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA10_I1T_02_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA10_MEM3m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA10_I1T_03_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA10_I1T_03_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_00_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_01_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM2m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_02_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_02_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM3m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_03_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_03_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM4m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_04_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_04_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM5m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_05_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_05_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_EFTA20_MEM6m",
        .dop_num = BCMPKT_TRACE_DOP_ID_EFTA20_I1T_06_INDEX_DOP,
        .group = 1,
        .fid = BCMPKT_TRACE_DOP_EFTA20_I1T_06_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA10_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA10_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA110_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA110_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA130_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA130_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA130_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA130_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA130_I1T_01_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA130_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA130_MEM2m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA130_I1T_02_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA130_I1T_02_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA130_MEM3m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA130_I1T_03_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA130_I1T_03_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA140_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA140_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA140_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_2_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_2_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_2_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_3_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_3_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD,
    },
    {
        .pt_name = "MEMDB_IFTA150_MEM0_3_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_IFTA20_MEM0_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA20_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA20_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA20_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA50_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA50_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA50_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA50_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA50_I1T_01_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA50_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA60_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA60_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA60_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA60_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA60_I1T_01_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA60_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA60_MEM2m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA60_I1T_02_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA60_I1T_02_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA70_MEM0m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA70_I1T_00_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA70_I1T_00_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_IFTA70_MEM1m",
        .dop_num = BCMPKT_TRACE_DOP_ID_IFTA70_I1T_01_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_IFTA70_I1T_01_INDEX_DOP_LKP0_LTPR_WIN,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_2_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_2_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_3_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA40_MEM0_3_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM0_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM0_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM1_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM1_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM2_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM2_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM3_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM3_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM4_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM4_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM5_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM5_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM6_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM6_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM7_0_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD,
    },
    {
        .pt_name = "MEMDB_TCAM_IFTA80_MEM7_1_ONLYm",
        .dop_num = BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP,
        .group = 0,
        .fid = BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH,
    },
};

static int bcm56780_a0_pkt_trace_init(int unit);

static void bcm56780_a0_reverse_word_wise(uint32_t *data, int sz);

static int bcm56780_a0_trace_dop_data_get(int unit, int port,
                                          uint32_t dop_id, uint8_t group_id,
                                          uint32_t *len, uint8_t *data);

static uint32_t bcm56780_a0_pkt_trace_pt_to_dop_num_get(int unit,
                                                        const char *pt_name,
                                                        const uint8_t *data,
                                                        uint32_t port,
                                                        uint32_t *dop_id);

static uint32_t bcm56780_a0_pkt_trace_pt_to_dop_group_get (int unit,
                                                           const char *pt_name,
                                                           const uint8_t *data,
                                                           uint32_t port,
                                                           uint32_t *group);

static uint32_t bcm56780_a0_pkt_trace_pt_to_dop_fid_get (int unit,
                                                         const char *pt_name,
                                                         const uint8_t *data,
                                                         uint32_t port,
                                                         uint32_t *fid);

static uint32_t bcm56780_a0_pkt_trace_pt_lookup_info_get (int unit,
                                                          const char *pt_name,
                                                          const uint8_t *data,
                                                          uint32_t port,
                                                          uint32_t *info);

static uint32_t bcm56780_a0_pkt_trace_pt_hit_info_get (int unit,
                                                       const char *pt_name,
                                                       const uint8_t *data,
                                                       uint32_t port,
                                                       uint32_t *pt_hit_info);

static int bcm56780_a0_pkt_trace_dop_num_comp (const void *e1, const void *e2);

extern const bcmpkt_trace_dop_fget_t bcm56780_a0_trace_dop_fget;
extern const bcmpkt_trace_dop_iget_t bcm56780_a0_trace_dop_iget;

const bcmpkt_trace_pt_to_dop_info_get_t bcm56780_a0_trace_pt_to_dop_info_get = {
    {
        bcm56780_a0_pkt_trace_pt_to_dop_num_get,
        bcm56780_a0_pkt_trace_pt_to_dop_group_get,
        bcm56780_a0_pkt_trace_pt_to_dop_fid_get,
        bcm56780_a0_pkt_trace_pt_lookup_info_get,
        bcm56780_a0_pkt_trace_pt_hit_info_get
    }
};

static int
bcm56780_a0_pkt_trace_pt_to_dop_info_search (const char *pt_name,
                                             int left, int right)
{
    int mid;
    int comp_val;

    if (left > right)  {
        return -1;
    }

    while (left <= right) {
        mid = left + (right - left)/2;

        comp_val = sal_strcmp(bcm56780_a0_pt_to_dop_map[mid].pt_name, pt_name);
        if (comp_val == 0) {
            return mid;
        }

        if (comp_val < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return SHR_E_NOT_FOUND;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_dop_info_get
 * Purpose:
 *      Maps a given global DOP number to chip specific local DOP ID.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      dop_num         - (IN)      Global DOP number
 *      dop_info        - (OUT)     DOP info
 * Returns:
 *      SHR_E_XXX
 */
static int
bcm56780_a0_pkt_trace_dop_info_get (int unit, uint32_t dop_num,
                                    bcmpkt_trace_dop_info_t *dop_info)
{
    bcmpkt_trace_global_to_local_dop_id_map_t key;
    const bcmpkt_trace_global_to_local_dop_id_map_t *map;

    key.global_dop_num = dop_num;
    map = sal_bsearch(&key, bcm56780_a0_trace_dop_id_map.dop_id_map,
                      COUNTOF(bcm56780_a0_trace_dop_id_map.dop_id_map),
                      sizeof(key), bcm56780_a0_pkt_trace_dop_num_comp);
    if (map) {
        dop_info->dop_id = map->local_dop_id;
        dop_info->group_id = map->group_id;
        return SHR_E_NONE;
    }

    return SHR_E_UNAVAIL;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_pt_to_dop_num_get
 * Purpose:
 *      Maps a given PT ID to DOP Id.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      pt_name         - (IN)      PT Name
 *      data            - (IN)      DOP Data
 *      port            - (IN)      port (unused)
 *      dop_num         - (OUT)     Global DOP Number
 * Returns:
 *      SHR_E_XXX
 */
static uint32_t
bcm56780_a0_pkt_trace_pt_to_dop_num_get (int unit, const char *pt_name,
                                         const uint8_t *data,
                                         uint32_t port,
                                         uint32_t *dop_num)
{
    int sz = sizeof(bcm56780_a0_pt_to_dop_map)/sizeof(bcmpkt_trace_pt_to_dop_map_t);
    int idx;

    idx = bcm56780_a0_pkt_trace_pt_to_dop_info_search(pt_name, 0, sz-1);
    if (idx >= 0) {
        *dop_num = bcm56780_a0_pt_to_dop_map[idx].dop_num;
    } else {
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_pt_to_dop_group_get
 * Purpose:
 *      Maps a given PT ID to DOP group.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      pt_name         - (IN)      PT Name
 *      data            - (IN)      DOP Data
 *      port            - (IN)      port (unused)
 *      group           - (OUT)     DOP Group ID
 * Returns:
 *      SHR_E_XXX
 */
static uint32_t
bcm56780_a0_pkt_trace_pt_to_dop_group_get (int unit, const char *pt_name,
                                           const uint8_t *data,
                                           uint32_t port, uint32_t *group)
{
    int sz;
    int idx;

    sz = sizeof(bcm56780_a0_pt_to_dop_map)/sizeof(bcmpkt_trace_pt_to_dop_map_t);
    idx = bcm56780_a0_pkt_trace_pt_to_dop_info_search(pt_name, 0, sz-1);

    if (idx >= 0) {
        *group = bcm56780_a0_pt_to_dop_map[idx].group;
    } else {
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_pt_to_dop_fid_get
 * Purpose:
 *      Maps a given PT ID to DOP Field ID.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      pt_name         - (IN)      PT Name
 *      data            - (IN)      DOP Data
 *      port            - (IN)      port (unused)
 *      fid             - (OUT)     DOP Field ID
 * Returns:
 *      SHR_E_XXX
 */
static uint32_t
bcm56780_a0_pkt_trace_pt_to_dop_fid_get (int unit, const char *pt_name,
                                         const uint8_t *data,
                                         uint32_t port, uint32_t *fid)
{
    int sz;
    int idx;

    sz = sizeof(bcm56780_a0_pt_to_dop_map)/sizeof(bcmpkt_trace_pt_to_dop_map_t);
    idx = bcm56780_a0_pkt_trace_pt_to_dop_info_search(pt_name, 0, sz-1);

    if (idx >= 0) {
        *fid = bcm56780_a0_pt_to_dop_map[idx].fid;
    } else {
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_pt_lookup_info_get
 * Purpose:
 *      The function output indicates if there was a lookup performed on
 *      a given PT ID.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      pt_name         - (IN)      PT Name
 *      data            - (IN)      DOP Data
 *      port            - (IN)      port
 *      fid             - (OUT)     PT lookup Info
 * Returns:
 *      SHR_E_XXX
 */
static uint32_t
bcm56780_a0_pkt_trace_pt_lookup_info_get (int unit, const char *pt_name,
                                          const uint8_t *data,
                                          uint32_t port,
                                          uint32_t *pt_lookup_info)
{
    uint32_t    mem_size;
    uint32_t    fid;
    int         idx, sz;
    uint32_t    *field_data;
    uint32_t    max_dop_phase;

    sz = sizeof(bcm56780_a0_pt_to_dop_map)/sizeof(bcmpkt_trace_pt_to_dop_map_t);
    idx = bcm56780_a0_pkt_trace_pt_to_dop_info_search(pt_name, 0, sz-1);
    max_dop_phase = bcm56780_a0_trace_dop_iget.\
                                        fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE]();

    if (idx >= 0) {
        fid = bcm56780_a0_pt_to_dop_map[idx].fid;

        mem_size = max_dop_phase * sizeof(uint32_t);
        field_data = (uint32_t *)sal_alloc(mem_size, "bcmpktDopFieldDumpTmp");
        bcm56780_a0_trace_dop_fget.fget[fid]((uint32_t *)data, field_data,
                                             mem_size);
        *pt_lookup_info = field_data[0];
        sal_free(field_data);
    } else {
        *pt_lookup_info = 0;
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_pt_hit_info_get
 * Purpose:
 *      The function output indicates if there was a PT hit for
 *      a given PT ID in last look up operation.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      pt_name         - (IN)      PT Name
 *      data            - (IN)      DOP data
 *      port            - (IN)      port
 *      fid             - (OUT)     PT hit Info
 * Returns:
 *      SHR_E_XXX
 */
static uint32_t
bcm56780_a0_pkt_trace_pt_hit_info_get (int unit, const char *pt_name,
                                       const uint8_t *data,
                                       uint32_t port,
                                       uint32_t *pt_hit_info)
{
    uint32_t    mem_size;
    uint32_t    fid;
    int         idx, sz;
    uint32_t    *field_data;
    uint32_t    max_dop_phase;

    if (sal_strstr(pt_name, "_DATA_ONLY") != NULL) {
        *pt_hit_info = 0;
        return SHR_E_NOT_FOUND;
    }

    sz = sizeof(bcm56780_a0_pt_to_dop_map)/sizeof(bcmpkt_trace_pt_to_dop_map_t);
    idx = bcm56780_a0_pkt_trace_pt_to_dop_info_search(pt_name, 0, sz-1);
    max_dop_phase = bcm56780_a0_trace_dop_iget.\
                                fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE]();

    if (idx >= 0) {
        fid = bcm56780_a0_pt_to_dop_map[idx].fid;

        mem_size = max_dop_phase * sizeof(uint32_t);
        field_data = (uint32_t *)sal_alloc(mem_size, "bcmpktDopFieldDumpTmp");
        bcm56780_a0_trace_dop_fget.fget[fid]((uint32_t *)data, field_data,
                                             mem_size);
        *pt_hit_info = field_data[0];
        sal_free(field_data);
    } else {
        *pt_hit_info = 0;
        return SHR_E_NOT_FOUND;
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_pkt_trace_dop_status_clear(int unit, uint32_t block,
                                       dop_ctrl_cmd_t cmd)
{
    uint32_t                ipep_map;
    int                     pipe;
    int                     ioerr = 0;
    IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r_t   ing_dop_status_0;
    EPOST_EGR_DOP_STATUS_0r_t                  egr_dop_status_0;
    int                     rv;

    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    switch (block) {
        case DOP_INGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r(unit, pipe,
                                                            &ing_dop_status_0);
                    switch (cmd) {
                        case DOP_CLEAR:
                            IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r_CLEAR_DONEf_SET(
                                                            ing_dop_status_0,
                                                            0);
                            break;
                        case DOP_INIT:
                            IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r_INIT_DONEf_SET(
                                                            ing_dop_status_0,
                                                            0);
                            break;
                        case DOP_SELF_TRIGGER:
                            IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r_SELF_TRIGGER_SETUP_DONEf_SET(
                                                            ing_dop_status_0,
                                                            0);
                            break;
                        case DOP_COLLECT:
                            IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r_COLLECTION_DONEf_SET(
                                                            ing_dop_status_0,
                                                            0);
                            break;
                    }
                    ioerr = WRITE_IPOST_SER_STATUS_BLK_ING_DOP_STATUS_0r(unit, pipe,
                                                            ing_dop_status_0);
                }
            }
            break;
        case DOP_EGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_EPOST_EGR_DOP_STATUS_0r(unit, pipe, &egr_dop_status_0);
                    switch (cmd) {
                        case DOP_CLEAR:
                            EPOST_EGR_DOP_STATUS_0r_CLEAR_DONEf_SET(
                                                            egr_dop_status_0,
                                                            0);
                            break;
                        case DOP_INIT:
                            EPOST_EGR_DOP_STATUS_0r_INIT_DONEf_SET(
                                                            egr_dop_status_0,
                                                            0);
                            break;
                        case DOP_SELF_TRIGGER:
                            EPOST_EGR_DOP_STATUS_0r_SELF_TRIGGER_SETUP_DONEf_SET(
                                                            egr_dop_status_0,
                                                            0);
                            break;
                        case DOP_COLLECT:
                            EPOST_EGR_DOP_STATUS_0r_COLLECTION_DONEf_SET(
                                                            egr_dop_status_0,
                                                            0);
                            break;
                    }
                    ioerr = WRITE_EPOST_EGR_DOP_STATUS_0r(unit, pipe,
                                                          egr_dop_status_0);
                }
            }
            break;
        default:
            return SHR_E_IO;
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
bcm56780_a0_pkt_trace_dop_reset(int unit, uint32_t block)
{
    uint32_t                ipep_map;
    int                     pipe;
    int                     ioerr = 0;
    ING_DOP_CTRL_0_64r_t    ing_dop_0_ctrl;
    EGR_DOP_CTRL_0_64r_t    egr_dop_0_ctrl;
    int                     rv;

    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    switch (block) {
        case DOP_INGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_ING_DOP_CTRL_0_64r(unit, pipe, &ing_dop_0_ctrl);
                    /* Set to capture_first mode */
                    ING_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(ing_dop_0_ctrl, 0);
                    /* Set command to DOP reset */
                    ING_DOP_CTRL_0_64r_COMMANDf_SET(ing_dop_0_ctrl, 0);
                    /* Issue DOP command */
                    ING_DOP_CTRL_0_64r_STARTf_SET(ing_dop_0_ctrl, 1);
                    ioerr += WRITE_ING_DOP_CTRL_0_64r(unit, pipe,
                                                      ing_dop_0_ctrl);
                }
            }
            break;
        case DOP_EGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_EGR_DOP_CTRL_0_64r(unit, pipe, &egr_dop_0_ctrl);
                    /* Set to capture_first mode */
                    EGR_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(egr_dop_0_ctrl, 0);
                    /* Set command to DOP reset */
                    EGR_DOP_CTRL_0_64r_COMMANDf_SET(egr_dop_0_ctrl, 0);
                    /* Issue DOP command */
                    EGR_DOP_CTRL_0_64r_STARTf_SET(egr_dop_0_ctrl, 1);
                    ioerr += WRITE_EGR_DOP_CTRL_0_64r(unit, pipe,
                                                      egr_dop_0_ctrl);
                }
            }
            break;
        default:
            break;
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
bcm56780_a0_pkt_trace_dop_init(int unit, uint32_t block,
                               uint32_t base_dop_id,
                               uint32_t dop_count)
{
    uint32_t                ipep_map;
    int                     pipe;
    int                     ioerr = 0;
    ING_DOP_CTRL_0_64r_t    ing_dop_0_ctrl;
    EGR_DOP_CTRL_0_64r_t    egr_dop_0_ctrl;
    ING_DOP_CTRL_2_64r_t    ing_dop_2_ctrl;
    EGR_DOP_CTRL_2_64r_t    egr_dop_2_ctrl;
    int                     rv;

    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    rv = bcm56780_a0_pkt_trace_dop_status_clear(unit, block, DOP_INIT);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    switch (block) {
        case DOP_INGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_ING_DOP_CTRL_2_64r(unit, pipe, &ing_dop_2_ctrl);
                    ING_DOP_CTRL_2_64r_DOP_IDf_SET(ing_dop_2_ctrl, base_dop_id);
                    ING_DOP_CTRL_2_64r_NUM_DOPSf_SET(ing_dop_2_ctrl, dop_count);
                    ioerr += WRITE_ING_DOP_CTRL_2_64r(unit, pipe,
                                                      ing_dop_2_ctrl);

                    READ_ING_DOP_CTRL_0_64r(unit, pipe, &ing_dop_0_ctrl);
                    /* Set to capture first mode */
                    ING_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(ing_dop_0_ctrl, 0);
                    /* Set command to DOP Init */
                    ING_DOP_CTRL_0_64r_COMMANDf_SET(ing_dop_0_ctrl, 1);
                    /* Issue DOP command */
                    ING_DOP_CTRL_0_64r_STARTf_SET(ing_dop_0_ctrl, 1);
                    ioerr += WRITE_ING_DOP_CTRL_0_64r(unit, pipe,
                                                      ing_dop_0_ctrl);
                }
            }
            break;
        case DOP_EGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_EGR_DOP_CTRL_2_64r(unit, pipe, &egr_dop_2_ctrl);
                    EGR_DOP_CTRL_2_64r_DOP_IDf_SET(egr_dop_2_ctrl, base_dop_id);
                    EGR_DOP_CTRL_2_64r_NUM_DOPSf_SET(egr_dop_2_ctrl, dop_count);
                    ioerr += WRITE_EGR_DOP_CTRL_2_64r(unit, pipe,
                                                      egr_dop_2_ctrl);

                    READ_EGR_DOP_CTRL_0_64r(unit, pipe, &egr_dop_0_ctrl);
                    /* Set to capture first mode */
                    EGR_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(egr_dop_0_ctrl, 0);
                    /* Set command to DOP Init */
                    EGR_DOP_CTRL_0_64r_COMMANDf_SET(egr_dop_0_ctrl, 1);
                    /* Issue DOP command */
                    EGR_DOP_CTRL_0_64r_STARTf_SET(egr_dop_0_ctrl, 1);
                    ioerr += WRITE_EGR_DOP_CTRL_0_64r(unit, pipe,
                                                      egr_dop_0_ctrl);
                }
            }
            break;
        default:
            break;
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static int
bcm56780_a0_pkt_trace_dop_collect(int unit,
                                  uint32_t block,
                                  int start_dop_id,
                                  int end_dop_id)
{
    uint32_t                ipep_map;
    int                     pipe;
    int                     ioerr = 0;
    ING_DOP_CTRL_0_64r_t    ing_dop_0_ctrl;
    EGR_DOP_CTRL_0_64r_t    egr_dop_0_ctrl;
    ING_DOP_CTRL_2_64r_t    ing_dop_2_ctrl;
    EGR_DOP_CTRL_2_64r_t    egr_dop_2_ctrl;
    IPOST_SER_STATUS_BLK_ING_DOP_STATUS_1r_t    ing_dop_status_1_ctrl;
    int                     rv;
    int                     dop_count;

    rv = bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /*
     * HW returns count + 1 numbers of DOP data. So for reading a single DOP,
     * this would be zero.
     */
    dop_count = end_dop_id - start_dop_id;

    switch (block) {
        case DOP_INGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    /* Start: Fix for BCMSIM crash */
                    READ_ING_DOP_CTRL_2_64r(unit, pipe, &ing_dop_2_ctrl);
                    /* Clear the STARTf field */
                    ING_DOP_CTRL_0_64r_STARTf_SET(ing_dop_0_ctrl, 0);
                    ioerr += WRITE_ING_DOP_CTRL_2_64r(unit, pipe,
                                                      ing_dop_2_ctrl);
                    READ_IPOST_SER_STATUS_BLK_ING_DOP_STATUS_1r(unit, pipe,
                                                      &ing_dop_status_1_ctrl);
                    IPOST_SER_STATUS_BLK_ING_DOP_STATUS_1r_WR_ADDRf_SET(
                                                      ing_dop_status_1_ctrl, 0);
                    ioerr += WRITE_IPOST_SER_STATUS_BLK_ING_DOP_STATUS_1r(unit,
                                                      pipe,
                                                      ing_dop_status_1_ctrl);
                    READ_ING_DOP_CTRL_2_64r(unit, pipe, &ing_dop_2_ctrl);
                    READ_IPOST_SER_STATUS_BLK_ING_DOP_STATUS_1r(unit, pipe,
                                                      &ing_dop_status_1_ctrl);
                    /* End: Fix for BCMSIM crash */

                    READ_ING_DOP_CTRL_2_64r(unit, pipe, &ing_dop_2_ctrl);
                    ING_DOP_CTRL_2_64r_DOP_IDf_SET(ing_dop_2_ctrl,
                                                   start_dop_id);
                    ING_DOP_CTRL_2_64r_NUM_DOPSf_SET(ing_dop_2_ctrl, dop_count);
                    ioerr += WRITE_ING_DOP_CTRL_2_64r(unit, pipe,
                                                      ing_dop_2_ctrl);

                    READ_ING_DOP_CTRL_0_64r(unit, pipe, &ing_dop_0_ctrl);
                    /* Set to capture first mode */
                    ING_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(ing_dop_0_ctrl, 0);
                    /* Set command to DOP collect */
                    ING_DOP_CTRL_0_64r_COMMANDf_SET(ing_dop_0_ctrl, 3);
                    /* Issue DOP command (Toggle STARTf bit) */
                    ING_DOP_CTRL_0_64r_STARTf_SET(ing_dop_0_ctrl, 0);
                    ioerr += WRITE_ING_DOP_CTRL_0_64r(unit, pipe,
                                                      ing_dop_0_ctrl);
                    ING_DOP_CTRL_0_64r_STARTf_SET(ing_dop_0_ctrl, 1);
                    ioerr += WRITE_ING_DOP_CTRL_0_64r(unit, pipe,
                                                      ing_dop_0_ctrl);
                }
            }
            break;
        case DOP_EGRESS:
            for (pipe = 0; pipe < NUM_PP_PIPES; pipe++) {
                if (ipep_map & (1 << pipe)) {
                    READ_EGR_DOP_CTRL_2_64r(unit, pipe, &egr_dop_2_ctrl);
                    EGR_DOP_CTRL_2_64r_DOP_IDf_SET(egr_dop_2_ctrl,
                                                   start_dop_id);
                    EGR_DOP_CTRL_2_64r_NUM_DOPSf_SET(egr_dop_2_ctrl, dop_count);
                    ioerr += WRITE_EGR_DOP_CTRL_2_64r(unit, pipe,
                                                      egr_dop_2_ctrl);

                    READ_EGR_DOP_CTRL_0_64r(unit, pipe, &egr_dop_0_ctrl);
                    /* Set to capture first mode */
                    EGR_DOP_CTRL_0_64r_CAPTURE_MODEf_SET(egr_dop_0_ctrl, 0);
                    /* Set command to DOP collect */
                    EGR_DOP_CTRL_0_64r_COMMANDf_SET(egr_dop_0_ctrl, 3);
                    /* Issue DOP command (Toggle STARTf bit) */
                    EGR_DOP_CTRL_0_64r_STARTf_SET(egr_dop_0_ctrl, 0);
                    ioerr += WRITE_EGR_DOP_CTRL_0_64r(unit, pipe,
                                                      egr_dop_0_ctrl);
                    EGR_DOP_CTRL_0_64r_STARTf_SET(egr_dop_0_ctrl, 1);
                    ioerr += WRITE_EGR_DOP_CTRL_0_64r(unit, pipe,
                                                      egr_dop_0_ctrl);
                }
            }
            break;
        default:
            break;
    }

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_dop_data_swap
 * Purpose:
 *      dop data swap
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      src_word_len    - (IN)      Number of words
 *      src             - (IN)      Input buffer
 *      dst             - (OUT)     Output buffer
 * Returns:
 *      SHR_E_XXX
 */
static int
bcm56780_a0_pkt_trace_dop_data_swap(int unit,
                                    uint32_t src_word_len,
                                    uint32_t *src,
                                    uint32_t *dst)
{
    uint32_t i = 0;

    if ((src == NULL)
        || (dst == NULL)){
        return SHR_E_INTERNAL;
    }

   for (i = 0; i <src_word_len; i++) {
        *(dst + (src_word_len - 1) - i) = *src++;
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *      bcm56780_a0_pkt_trace_dop_parse
 * Purpose:
 *      DOP parsing.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      buf             - (IN/OUT)  Trace data buffer
 *      dop_count       - (IN)      dop count
 *      size            - (IN)      Input trace data size
 *      data_sz         - (OUT)     Parsed data size
 * Returns:
 *      SHR_E_XXX
 */
static int
bcm56780_a0_pkt_trace_dop_parse(int unit,
                                 uint32_t *buf,
                                 int      dop_count,
                                 uint32_t size,
                                 uint32_t *data_sz)
{
    uint32_t resp_word;
    uint32_t dop_id = 0;
    uint32_t data_valid = 0;
    uint32_t data_phases = 0;
    uint32_t *dop_rd_ptr = NULL;
    uint32_t *dop_wr_ptr = NULL;
    uint32_t *temp_entry;
    uint16_t byte_cnt = 0;
    int      rv;

    dop_rd_ptr = buf;
    dop_wr_ptr = buf;
    *data_sz = 0;
    while (byte_cnt < size) {
        /* Read the first word of the dop_data i.e response word */
        sal_memcpy(&resp_word, dop_rd_ptr, sizeof(resp_word));
        dop_count--;
        dop_rd_ptr++;
        byte_cnt += sizeof(resp_word);
        bcmdrd_pt_field_get(unit, DOP_COLLECTION_RESP_WORD,
                            (uint32_t *)&resp_word,
                            DOP_IDf, (uint32_t *)(&dop_id));
        bcmdrd_pt_field_get(unit, DOP_COLLECTION_RESP_WORD,
                            (uint32_t *)&resp_word,
                            DATA_PHASESf, (uint32_t *)(&data_phases));
        bcmdrd_pt_field_get(unit, DOP_COLLECTION_RESP_WORD,
                            (uint32_t *)&resp_word,
                            DATA_VALIDf, (uint32_t *)(&data_valid));

        if ((data_phases != 0) && (data_valid == 1)) {
            temp_entry = sal_alloc(WORDS2BYTES(data_phases),
                                   "bcmpktTraceDopEntry");
            sal_memset(temp_entry, 0, WORDS2BYTES(data_phases));
            if (temp_entry == NULL) {
                return SHR_E_MEMORY;
            }

            sal_memcpy(temp_entry, dop_rd_ptr, WORDS2BYTES(data_phases));
            rv = bcm56780_a0_pkt_trace_dop_data_swap(unit, data_phases,
                                                temp_entry, dop_wr_ptr);
            if (SHR_FAILURE(rv)) {
                sal_free(temp_entry);
                return rv;
            }
            dop_wr_ptr += data_phases;
            (*data_sz) += (data_phases * sizeof(uint32_t));
            dop_rd_ptr += data_phases;
            byte_cnt += (data_phases * sizeof(uint32_t));
            sal_free(temp_entry);
            if (dop_count == 0) {
                break;
            }
        } else {
            break;
        }
    };

    return SHR_E_NONE;
}

static int
bcm56780_a0_pkt_trace_dop_num_comp(const void *e1, const void *e2)
{
    int x = ((bcmpkt_trace_global_to_local_dop_id_map_t *)e1)->global_dop_num;
    int y = ((bcmpkt_trace_global_to_local_dop_id_map_t *)e2)->global_dop_num;

    return (int)(x - y);
}

static int
bcm56780_a0_pkt_trace_init(int unit)
{
    uint32_t    block = 0;
    int         ioerr = 0;

    /* Sort the entries in the array */
    sal_qsort((void *)bcm56780_a0_trace_dop_id_map.dop_id_map,
              COUNTOF(bcm56780_a0_trace_dop_id_map.dop_id_map),
              sizeof(bcm56780_a0_trace_dop_id_map.dop_id_map[0]),
              bcm56780_a0_pkt_trace_dop_num_comp);

    block = DOP_INGRESS;
    /* initalize the INGRESS DOPs */
    ioerr += bcm56780_a0_pkt_trace_dop_reset(unit, block);
    ioerr += bcm56780_a0_pkt_trace_dop_init(unit, block, 0,
                                            MAX_ING_DOP_FORMATS(unit));

    block = DOP_EGRESS;
    ioerr += bcm56780_a0_pkt_trace_dop_reset(unit, block);
    ioerr += bcm56780_a0_pkt_trace_dop_init(unit, block, 0,
                                            MAX_EGR_DOP_FORMATS(unit));
    return ioerr ? SHR_E_IO : SHR_E_NONE;
}


static int
bcm56780_a0_trace_init(int unit)
{
    int     ioerr = 0;

    if (!bcmbd_attached(unit)) {
        return SHR_E_INIT;
    }

    ioerr += bcm56780_a0_pkt_trace_init(unit);

    return ioerr ? SHR_E_IO : SHR_E_NONE;
}

static void
bcm56780_a0_reverse_word_wise(uint32_t *data, int sz)
{
    int i;
    uint32_t tmp;

    if (!data) {
        return;
    }

    for (i=0; i<(sz/2); i++) {
        tmp = data[i];
        data[i] = data[sz-i-1];
        data[sz-i-1] = tmp;
    }

    return;
}

static int
bcm56780_a0_pkt_trace_raw_data_get(int unit, int port, uint32_t block,
                                   int dop_count, uint32_t *len, uint8_t *data)
{
    int pipe_num;
    uint16_t index, max_idx;
    uint32_t rsp_flags, mem_width;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    uint32_t *entry_buf;
    uint32_t mem_size;
    uint32_t max_dop_phase;

    SHR_FUNC_ENTER(unit);

    pipe_num = bcmdrd_dev_logic_port_pipe(unit, port);
    if (pipe_num < 0) {
        return SHR_E_PARAM;
    }

    /* Convert logical to Physical pipe Number */
    pipe_num /= 2;

    pt_dyn_info.tbl_inst = pipe_num;

    if (block == DOP_EGRESS) {
        sid = EPOST_EGR_DOP_STORAGE_MEMm;
    } else {
        sid = IPOST_SER_STATUS_BLK_ING_DOP_STORAGE_MEMm;
    }

    mem_width = bcmdrd_pt_entry_wsize(unit, sid);
    if (mem_width == 0) {
        return SHR_E_FAIL;
    }

    max_idx = bcmdrd_pt_index_max(unit, sid);
    req_ltid = sid;
    mem_size = (mem_width * sizeof(uint32_t)) * max_idx;
    entry_buf = sal_alloc(mem_width * sizeof(uint32_t), "bcmpktTraceEntryBuf");

    /* If single DOP read, set the max_idx to maximum DOP phase value */
    if (dop_count == 1) {
        max_dop_phase = bcm56780_a0_trace_dop_iget.\
                                fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE]();
        max_idx = max_dop_phase/mem_width;
    }

    for (index=0; index < max_idx; index++) {
        pt_dyn_info.index = index;
        sal_memset(entry_buf, 0, mem_width * sizeof(uint32_t));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              BCMPTM_REQ_FLAGS_PASSTHRU,
                              sid,
                              &pt_dyn_info,
        NULL,
                              mem_width,
                              req_ltid,
                              entry_buf,
                              &rsp_ltid,
                              &rsp_flags));

        bcmdrd_pt_field_get(unit, sid, entry_buf, DOP_DATAf,
                            (uint32_t *)(data + (index * 16)));

        bcm56780_a0_reverse_word_wise((uint32_t *)(data + (index * 16)), 4);
    }

    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_parse(unit, (uint32_t *)data,
                                                    dop_count, mem_size, len));

exit:
    sal_free(entry_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trace_data_get(int unit, int port, uint32_t *len, uint8_t *data)
{
    uint8_t         *tmp_data;
    uint32_t        data_len=0;
    bcmdrd_sid_t    sid;
    uint32_t        mem_size;
    uint16_t        max_idx;
    uint32_t        mem_width;
    uint32_t        base_dop_id;
    uint32_t        dop_count;

    SHR_FUNC_ENTER(unit);

    sid = IPOST_SER_STATUS_BLK_ING_DOP_STORAGE_MEMm;
    mem_width = bcmdrd_pt_entry_wsize(unit, sid);
    max_idx = bcmdrd_pt_index_max(unit, sid);
    mem_size = (mem_width * sizeof(uint32_t)) * max_idx;

    tmp_data = sal_alloc(mem_size, "bcmpktTraceData");
    sal_memset(tmp_data, 0, mem_size);

    /*
     * Read first MAX_ING_DOP_READ_COUNT of  INGRESS DOPs as the H/W Ingress
     * DOP storage memory can not accomodate all the DOPs in one read operation.
     * Also note that the end DOP ID in each read is one less than total
     * number of DOPS we want to read. This is a HW issue where it returns
     * one more than the requested DOP count.
     */
    base_dop_id = BASE_DOP_ID;
    dop_count = MAX_ING_DOP_READ_COUNT;
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_collect(unit, DOP_INGRESS,
                                                      base_dop_id,
                                                      (dop_count-1)));
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_raw_data_get(unit, port,
                                                       DOP_INGRESS, dop_count,
                                                       &data_len, tmp_data));
    *len = data_len;
    sal_memcpy(data, tmp_data, data_len);

    /* Read the remaining Ingress DOPs.*/
    data_len = 0;
    sal_memset(tmp_data, 0, mem_size);
    base_dop_id = BASE_DOP_ID + MAX_ING_DOP_READ_COUNT;
    dop_count = MAX_ING_DOP_FORMATS(unit) - MAX_ING_DOP_READ_COUNT;
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_collect(unit, DOP_INGRESS,
                                                      base_dop_id,
                                                      (dop_count-1)));
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_raw_data_get(unit, port,
                                                       DOP_INGRESS, dop_count,
                                                       &data_len, tmp_data));
    sal_memcpy(data+(*len), tmp_data, data_len);
    *len += data_len;

    /* Read the All the egress DOPs in one go */
    sid = EPOST_EGR_DOP_STORAGE_MEMm;
    mem_width = bcmdrd_pt_entry_wsize(unit, sid);
    max_idx = bcmdrd_pt_index_max(unit, sid);
    mem_size = (mem_width * sizeof(uint32_t)) * max_idx;

    data_len = 0;
    sal_memset(tmp_data, 0, mem_size);
    base_dop_id = BASE_DOP_ID;
    dop_count = MAX_EGR_DOP_FORMATS(unit);
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_collect(unit, DOP_EGRESS,
                                                      base_dop_id,
                                                      (dop_count-1)));
    SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_raw_data_get(unit, port,
                                                       DOP_EGRESS, dop_count,
                                                       &data_len, tmp_data));
    sal_memcpy(data+(*len), tmp_data, data_len);
    *len += data_len;

exit:
    sal_free(tmp_data);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm56780_a0_trace_dop_data_get
 * Purpose:
 *      The function output DOP Data for a given DOP ID, group
 *      and port number.
 * Parameters:
 *      unit            - (IN)      BCM device number
 *      port            - (IN)      port
 *      dop_id          - (IN)      DOP ID
 *      group_id        - (IN)      DOP Group ID
 *      len             - (OUT)     DOP Data output len
 *      data            - (OUT)     DOP Data output buffer
 * Returns:
 *      SHR_E_XXX
 */
static int
bcm56780_a0_trace_dop_data_get(int unit, int port, uint32_t dop_id,
                               uint8_t group_id,
                               uint32_t *len, uint8_t *data)
{
    uint8_t         *tmp_data;
    uint32_t        data_len=0;
    uint32_t        mem_size;
    uint32_t        base_dop_id;
    uint32_t        max_dop_phase;

    SHR_FUNC_ENTER(unit);

    max_dop_phase = bcm56780_a0_trace_dop_iget.\
                                fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE]();

    if (group_id == 0) {
        mem_size = max_dop_phase * sizeof(uint32_t);
        tmp_data = sal_alloc(mem_size, "bcmDop");
        sal_memset(tmp_data, 0, mem_size);

        base_dop_id = dop_id;
        SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_collect(unit, DOP_INGRESS,
                                                   base_dop_id, base_dop_id));
        SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_raw_data_get(unit, port,
                                                           DOP_INGRESS, 1,
                                                           &data_len,
                                                           tmp_data));
        *len = data_len;
        sal_memcpy(data, tmp_data, data_len);
    } else if (group_id == 1) {
        mem_size = max_dop_phase * sizeof(uint32_t);
        tmp_data = sal_alloc(mem_size, "bcmDop");
        sal_memset(tmp_data, 0, mem_size);

        base_dop_id = dop_id;
        SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_dop_collect(unit, DOP_EGRESS,
                                                   base_dop_id, base_dop_id));
        SHR_IF_ERR_EXIT(bcm56780_a0_pkt_trace_raw_data_get(unit, port,
                                                           DOP_EGRESS, 1,
                                                           &data_len,
                                                           tmp_data));
        sal_memcpy(data, tmp_data, data_len);
        *len = data_len;
    } else {
        return SHR_E_PARAM;
    }

exit:
    sal_free(tmp_data);
    SHR_FUNC_EXIT();
}
static int
bcm56780_a0_trace_get_txpmd_val(uint32_t dev_type, int fid, int *val)
{
    if (fid == BCMPKT_TXPMD_SET_L2BM) {
        *val = 0;
    }

    if (fid == BCMPKT_TXPMD_UNICAST) {
        *val = 1;
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_trace_get_lbpipe(int unit, int pipe, int *lbpipe)
{
    if (pipe%2 == 0) {
        *lbpipe = pipe + 1;
    }

    return SHR_E_NONE;
}

int
bcm56780_a0_trace_drv_attach(bcmpkt_trace_drv_t *trace_drv)
{
    if (trace_drv == NULL) {
        return SHR_E_FAIL;
    }

    trace_drv->profile_init = bcm56780_a0_trace_init;
    trace_drv->data_read = bcm56780_a0_trace_data_get;
    trace_drv->get_lbpipe = bcm56780_a0_trace_get_lbpipe;
    trace_drv->dop_data_read = bcm56780_a0_trace_dop_data_get;
    trace_drv->dop_info_get = bcm56780_a0_pkt_trace_dop_info_get;
    trace_drv->get_txpmd_val = bcm56780_a0_trace_get_txpmd_val;
    trace_drv->counter_shift = NULL;

    return SHR_E_NONE;
}
