/*! \file bcm56990_a0_cth_ctr_eflex.c
 *
 * Chip stub for BCMCTH CTR EFLEX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>

#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*!
 * Maximum ACTION_TABLE_0 PT entry size
 * 116 bits == 4 uint32_t
 */
#define ACTION_TABLE_0_PT_WSIZE 4

/*!
 * Maximum ACTION_TABLE_1 PT entry size
 * 21 bits == 1 uint32_t
 */
#define ACTION_TABLE_1_PT_WSIZE 1

/*!
 * Maximum OP_PROFILE PT entry size
 * 26 bits == 1 uint32_t
 */
#define OP_PROFILE_PT_WSIZE 1

/*!
 * Maximum RANGE_CHECK PT entry size
 * 32 bits == 1 uint32_t
 */
#define RANGE_CHECK_PT_WSIZE 1

/*! Counter number per section in a pool */
#define NUM_CTR_SECTION 128

/*! Pipe number */
#define NUM_PIPE 4

/*! Pipe instance number */
#define NUM_PIPE_INST 2

/*! Ingress action number */
#define NUM_ING_ACTION 32

/*! Egress action number */
#define NUM_EGR_ACTION 16

/*! Ingress group action number */
#define NUM_ING_GROUP_ACTION 4

/*! Egress group action number */
#define NUM_EGR_GROUP_ACTION 4

/*! Operand number */
#define NUM_OPERAND 64

/*! Range checker number */
#define NUM_RANGE_CHECKER 32

/*! Bank number */
#define NUM_RANGE_CHECKER_PER_BANK 8

/*! Object number per action */
#define NUM_OBJECT 4

/*! Global Object number per action */
#define NUM_GLOBAL_OBJECT 0

/*! Number of flex counter simultaneous ingress actions */
#define NUM_CTR_EFLEX_ING_SIM_ACTION 11

/*! Number of flex counter simultaneous egress actions */
#define NUM_CTR_EFLEX_EGR_SIM_ACTION 7

/*! Number of slim counters per entry. */
#define NUM_SLIM_CTRS 3

/*! Slim counter is organised coloumn wise. */
#define SLIM_CTR_ROW FALSE

typedef struct obj_container_s {
    uint32_t id;
    uint32_t start;
    uint32_t end;
    uint32_t width;
    char *desc;
} obj_container_t;

enum ing_ctr_tbl_e {
    FLEX_CTR_ING_COUNTER_TABLE_0m = FLEX_CTR_ING_COUNTER_TABLE_0_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_1m = FLEX_CTR_ING_COUNTER_TABLE_1_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_2m = FLEX_CTR_ING_COUNTER_TABLE_2_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_3m = FLEX_CTR_ING_COUNTER_TABLE_3_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_4m = FLEX_CTR_ING_COUNTER_TABLE_4_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_5m = FLEX_CTR_ING_COUNTER_TABLE_5_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_6m = FLEX_CTR_ING_COUNTER_TABLE_6_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_7m = FLEX_CTR_ING_COUNTER_TABLE_7_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_8m = FLEX_CTR_ING_COUNTER_TABLE_8_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_9m = FLEX_CTR_ING_COUNTER_TABLE_9_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_10m = FLEX_CTR_ING_COUNTER_TABLE_10_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_11m = FLEX_CTR_ING_COUNTER_TABLE_11_INST0m
};

enum egr_ctr_tbl_e {
    FLEX_CTR_EGR_COUNTER_TABLE_0m = FLEX_CTR_EGR_COUNTER_TABLE_0_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1m = FLEX_CTR_EGR_COUNTER_TABLE_1_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_2m = FLEX_CTR_EGR_COUNTER_TABLE_2_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_3m = FLEX_CTR_EGR_COUNTER_TABLE_3_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_4m = FLEX_CTR_EGR_COUNTER_TABLE_4_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_5m = FLEX_CTR_EGR_COUNTER_TABLE_5_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_6m = FLEX_CTR_EGR_COUNTER_TABLE_6_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_7m = FLEX_CTR_EGR_COUNTER_TABLE_7_INST0m,
};

static bcmdrd_sid_t ingress_counter_table[] = {
    FLEX_CTR_ING_COUNTER_TABLE_0m,
    FLEX_CTR_ING_COUNTER_TABLE_1m,
    FLEX_CTR_ING_COUNTER_TABLE_2m,
    FLEX_CTR_ING_COUNTER_TABLE_3m,
    FLEX_CTR_ING_COUNTER_TABLE_4m,
    FLEX_CTR_ING_COUNTER_TABLE_5m,
    FLEX_CTR_ING_COUNTER_TABLE_6m,
    FLEX_CTR_ING_COUNTER_TABLE_7m,
    FLEX_CTR_ING_COUNTER_TABLE_8m,
    FLEX_CTR_ING_COUNTER_TABLE_9m,
    FLEX_CTR_ING_COUNTER_TABLE_10m,
    FLEX_CTR_ING_COUNTER_TABLE_11m
};

static bcmdrd_sid_t egress_counter_table[] = {
    FLEX_CTR_EGR_COUNTER_TABLE_0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1m,
    FLEX_CTR_EGR_COUNTER_TABLE_2m,
    FLEX_CTR_EGR_COUNTER_TABLE_3m,
    FLEX_CTR_EGR_COUNTER_TABLE_4m,
    FLEX_CTR_EGR_COUNTER_TABLE_5m,
    FLEX_CTR_EGR_COUNTER_TABLE_6m,
    FLEX_CTR_EGR_COUNTER_TABLE_7m,
};

static bcmdrd_sid_t ingress_counter_inst_table[] = {
    FLEX_CTR_ING_COUNTER_TABLE_0_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_1_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_2_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_3_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_4_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_5_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_6_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_7_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_8_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_9_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_10_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_11_INST0m,
    FLEX_CTR_ING_COUNTER_TABLE_0_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_1_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_2_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_3_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_4_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_5_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_6_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_7_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_8_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_9_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_10_INST1m,
    FLEX_CTR_ING_COUNTER_TABLE_11_INST1m
};

static bcmdrd_sid_t egress_counter_inst_table[] = {
    FLEX_CTR_EGR_COUNTER_TABLE_0_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_1_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_2_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_3_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_4_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_5_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_6_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_7_INST0m,
    FLEX_CTR_EGR_COUNTER_TABLE_0_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_1_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_2_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_3_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_4_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_5_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_6_INST1m,
    FLEX_CTR_EGR_COUNTER_TABLE_7_INST1m
};

static bcmdrd_sid_t ing_group_action_num_inst[] =
{
    FLEX_CTR_ING_GROUP_ACTION_0_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_1_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_2_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_3_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_0_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_1_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_2_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_3_INST1r
};

static bcmdrd_sid_t egr_group_action_num_inst[] =
{
    FLEX_CTR_EGR_GROUP_ACTION_0_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_1_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_2_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_3_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_0_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_1_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_2_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_3_INST1r
};

static bcmdrd_sid_t ing_group_action_bitmap_inst[] =
{
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_0_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_1_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_2_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_3_INST0r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_0_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_1_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_2_INST1r,
    FLEX_CTR_ING_GROUP_ACTION_BITMAP_3_INST1r
};

static bcmdrd_sid_t egr_group_action_bitmap_inst[] =
{
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_0_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_1_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_2_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_3_INST0r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_0_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_1_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_2_INST1r,
    FLEX_CTR_EGR_GROUP_ACTION_BITMAP_3_INST1r
};

static obj_container_t ing_obj_fmt_tbl[] =
{
    {0, 0, 9, 10, "VFP policy table slice 0"},
    {1, 16, 25, 10, "VFP policy table slice 1"},
    {2, 32, 41, 10, "VFP policy table slice 2"},
    {3, 48, 57, 10, "VFP policy table slice 3"},
    {4, 64, 75, 10, "IFP policy table slice 0"},
    {5, 80, 91, 10, "IFP policy table slice 1"},
    {6, 96, 107, 10, "IFP policy table slice 2"},
    {7, 112, 123, 10, "IFP policy table slice 3"},
    {8, 128, 139, 10, "IFP policy table slice 4"},
    {9, 144, 155, 10, "IFP policy table slice 5"},
    {10, 160, 171, 10, "IFP policy table slice 6"},
    {11, 176, 187, 10, "IFP policy table slice 7"},
    {12, 192, 203, 10, "IFP policy table slice 8"},
    {13, 208, 221, 10, "Exact match policy table"},
    {14, 224, 237, 10, "MPLS entry 1 object"},
    {15, 240, 253, 10, "MPLS entry 2 object"},
    {24, 0, 9, 10, "L3 tunnel"},
    {16, 0, 9, 10, "Prefix length for flow count"},
    {63, 0, 9, 10, "Port context for flow count"},
    {27, 0, 9, 10, "Flow size lower byte for flow count"},
    {28, 0, 9, 10, "Flow size upper byte for flow count"},
    {18, 0, 9, 10, "ALPM source lookup"},
    {19, 0, 9, 10, "ALPM destination lookup"},
    {20, 0, 9, 10, "Lookup hit status"},
    {21, 0, 9, 10, "Ingress port"},
    {22, 0, 9, 10, "Egress port"},
    {23, 0, 9, 10, "VLAN ID"},
    {24, 0, 9, 10, "L3 IIF"},
    {25, 0, 9, 10, "VRF"},
    {26, 0, 9, 10, "Class ID from port table"},
    {29, 0, 9, 10, "ECMP group"},
    {30, 0, 9, 10, "ECMP member"},
    {31, 0, 9, 10, "Next hop or MC index"},
    {32, 0, 9, 10, "RTAG7 hash 0 and instance 0"},
    {33, 0, 9, 10, "RTAG7 hash 0 and instance 1"},
    {34, 0, 9, 10, "RTAG7 hash 1 and instance 0"},
    {35, 0, 9, 10, "RTAG7 hash 1 and instance 1"},
    {36, 0, 9, 10, "Ingress timestamp 0-15 bits"},
    {37, 0, 9, 10, "Ingress timestamp 16-31 bits"},
    {38, 0, 9, 10, "Ingress timestamp 32-47 bits"},
    {39, 0, 9, 10, "Packet length in IP header"},
    {40, 0, 9, 10, "L4 flags"},
    {41, 0, 9, 10, "UDF1 chunk 0"},
    {42, 0, 9, 10, "UDF1 chunk 1"},
    {43, 0, 9, 10, "UDF1 chunk 2"},
    {44, 0, 9, 10, "UDF1 chunk 3"},
    {45, 0, 9, 10, "UDF1 chunk 4"},
    {46, 0, 9, 10, "UDF1 chunk 5"},
    {47, 0, 9, 10, "UDF1 chunk 6"},
    {48, 0, 9, 10, "UDF1 chunk 7"},
    {49, 0, 9, 10, "UDF2 chunk 0"},
    {50, 0, 9, 10, "UDF2 chunk 1"},
    {51, 0, 9, 10, "UDF2 chunk 2"},
    {52, 0, 9, 10, "UDF2 chunk 3"},
    {53, 0, 9, 10, "UDF2 chunk 4"},
    {54, 0, 9, 10, "UDF2 chunk 5"},
    {55, 0, 9, 10, "UDF2 chunk 6"},
    {56, 0, 9, 10, "UDF2 chunk 7"},
    {57, 0, 9, 10, "Packet attribute offset 0"},
    {58, 0, 9, 10, "Packet attribute offset 1"},
    {59, 0, 9, 10, "Packet attribute offset 2"},
    {60, 0, 9, 10, "Internal priority"},
    {60, 0, 9, 10, "Internal congestion notification"},
    {60, 0, 9, 10, "Ingress mirror on drop event"},
    {61, 0, 9, 10, "Trunk group"},
    {61, 0, 9, 10, "Trunk member"},
    {62, 0, 9, 10, "Protocol field from IP header"},
};

static obj_container_t egr_obj_fmt_tbl[] =
{
    {0, 0, 13, 14, "Egress adapt table"},
    {1, 0, 13, 14, "EFP policy table slice 0"},
    {2, 0, 13, 14, "EFP policy table slice 1"},
    {3, 0, 13, 14, "EFP policy table slice 2"},
    {4, 0, 13, 14, "EFP policy table slice 3"},
    {5, 0, 13, 14, "Ingress port"},
    {6, 0, 13, 14, "Egress port"},
    {7, 0, 13, 14, "Vlan ID"},
    {8, 0, 13, 14, "VRF"},
    {9, 0, 13, 14, "L3 Outgoing interface"},
    {10, 0, 13, 14, "L3 tunnel index"},
    {11, 0, 13, 14, "Next hop"},
    {12, 0, 13, 14, "Metadata from egress NHOP table"},
    {13, 0, 13, 14, "I2E class ID"},
    {13, 0, 13, 14, "I2E class ID type"},
    {14, 0, 13, 14, "Queue number"},
    {15, 0, 13, 14, "Queue occupancy stats 0"},
    {17, 0, 13, 14, "Queue occupancy stats 1"},
    {19, 0, 13, 14, "Residency timestamp lower"},
    {20, 0, 13, 14, "Residency timestamp upper"},
    {21, 0, 13, 14, "Egress timestamp 0-15 bits"},
    {22, 0, 13, 14, "Egress timestamp 16-31 bits"},
    {23, 0, 13, 14, "Egress timestamp 32-47 bits"},
    {24, 0, 13, 14, "Ingress timestamp 0-15 bits"},
    {25, 0, 13, 14, "Ingress timestamp 16-31 bits"},
    {26, 0, 13, 14, "Ingress timestamp 32-47 bits"},
    {27, 0, 13, 14, "Entropy label 0-15 bits"},
    {28, 0, 13, 14, "Entropy label 16-25 bits"},
    {29, 0, 13, 14, "Egress mirror on drop event"},
    {29, 0, 13, 14, "Internal congestion notification"},
    {29, 0, 13, 14, "Internal priority"},
    {29, 0, 13, 14, "Congestion color"},
    {30, 0, 13, 14, "Packet attribute offset instance 0"},
    {31, 0, 13, 14, "Packet attribute offset instance 1"},
    {32, 0, 13, 14, "Packet attribute offset instance 2"},
    {33, 0, 13, 14, "Combine egress port and queue number"},
};

static bcmdrd_sid_t ingress_update_control_reg[] = {
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_0_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_1_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_2_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_3_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_4_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_5_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_6_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_7_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_8_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_9_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_10_INST0r,
    FLEX_CTR_ING_COUNTER_UPDATE_CONTROL_POOL_11_INST0r
};

static bcmdrd_sid_t egress_update_control_reg[] = {
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_0_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_1_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_2_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_3_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_4_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_5_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_6_INST0r,
    FLEX_CTR_EGR_COUNTER_UPDATE_CONTROL_POOL_7_INST0r
};

static bcmcth_ctr_eflex_device_info_t bcm56990_a0_device_info = {
    .num_ingress_pools = COUNTOF(ingress_counter_table),
    .num_egress_pools = COUNTOF(egress_counter_table),
    .num_ctr_section = NUM_CTR_SECTION,
    .num_pipes = NUM_PIPE,
    .num_pipe_instance = NUM_PIPE_INST,
    .num_ingress_action_id = NUM_ING_ACTION,
    .num_egress_action_id = NUM_EGR_ACTION,
    .num_op_profile_id = NUM_OPERAND,
    .num_range_checker = NUM_RANGE_CHECKER,
    .num_range_checker_per_bank = NUM_RANGE_CHECKER_PER_BANK,
    .num_objs = NUM_OBJECT,
    .num_global_objs = NUM_GLOBAL_OBJECT,
    .num_slim_ctrs = NUM_SLIM_CTRS,
    .num_ing_actions = NUM_CTR_EFLEX_ING_SIM_ACTION,
    .num_egr_actions = NUM_CTR_EFLEX_EGR_SIM_ACTION,
    .slim_ctr_row = SLIM_CTR_ROW,
};

/*******************************************************************************
* Private functions
 */

/*! Get list of COUNTER_TABLEm. */
static int
bcm56990_a0_cth_ctr_eflex_ctr_table_get(int unit,
                                        bool ingress,
                                        bcmdrd_sid_t **ctr_table_list,
                                        uint32_t *num)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num, SHR_E_PARAM);

    if (ingress) {
        *ctr_table_list = ingress_counter_inst_table;
        *num = COUNTOF(ingress_counter_inst_table);
    } else {
        *ctr_table_list = egress_counter_inst_table;
        *num = COUNTOF(egress_counter_inst_table);
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Get device and pool info */
static int
bcm56990_a0_cth_ctr_eflex_device_info_get(
    int unit,
    bcmcth_ctr_eflex_device_info_t **device_info)
{
    SHR_FUNC_ENTER(unit);

    *device_info = &bcm56990_a0_device_info;

    SHR_FUNC_EXIT();
}

/*! Set counter action enable reg */
static int
bcm56990_a0_cth_ctr_eflex_action_reg_set(int unit,
                                         bool enable,
                                         ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t *reg_buf = NULL;
    uint32_t wsize, pipe_idx;
    size_t size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct register based on entry ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_ENABLE_INST0r :
                           FLEX_CTR_EGR_COUNTER_ACTION_ENABLE_INST0r;
    inst = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_ENABLE_INST1r :
                            FLEX_CTR_EGR_COUNTER_ACTION_ENABLE_INST1r;

    wsize = bcmdrd_pt_entry_wsize(unit, sid);

    size = wsize * sizeof(uint32_t);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(reg_buf, size, "bcmCthCtrEflexRegBuf");
    SHR_NULL_CHECK(reg_buf, SHR_E_MEMORY);
    sal_memset(reg_buf, 0, size);

    /* Set action enable register for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        sal_memset(reg_buf, 0, size);

        /* Read the current reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, sid, &pt_dyn_info, NULL, reg_buf, wsize));

        if (enable) {
            /* Set the bit for specified action id */
            SHR_BITSET(reg_buf, entry->action_id);
        } else {
            /* Clear the bit for specified action id */
            SHR_BITCLR(reg_buf, entry->action_id);
        }

        /* Write the updated reg value */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, sid, &pt_dyn_info, NULL, reg_buf));
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, inst, &pt_dyn_info, NULL, reg_buf));
    }

exit:
    SHR_FREE(reg_buf);
    SHR_FUNC_EXIT();
}

/*! Write counter action table 0 entry */
static int
bcm56990_a0_cth_ctr_eflex_action_table_0_write(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t entry_buf[ACTION_TABLE_0_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0, pipe_idx;
    uint32_t count_on_mirror;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST0m;
    inst = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST1m :
                            FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST1m;

    /* Write action table 0 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

         /* Ignore disabled pipes */
         if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
             continue;
         }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /*
         * Read current PT entry.
         * Part of ACTION_TABLE_0 entry can be written by
         * CTR_xxx_EFLEX_TRIGGER LT
         */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              entry->req_flags,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              (ACTION_TABLE_0_PT_WSIZE * sizeof(uint32_t)),
                              req_ltid,
                              entry_buf,
                              &rsp_ltid,
                              &rsp_flags));

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_0f,
                                 &entry->obj[0]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_1f,
                                 &entry->obj[1]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_2f,
                                 &entry->obj[2]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, SIZE0_SEL_3f,
                                 &entry->obj[3]));

        /* For ingress, count on mirror is always set to 1. */
        count_on_mirror = entry->ingress ? 1 : entry->count_on_mirror_egress;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_MIRRORf,
                                        &count_on_mirror));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_DROPf,
                                         &entry->count_on_rule_drop));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, POOL_BASEf,
                                         &entry->pool_idx));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNTER_MODEf,
                                         &entry->ctr_mode));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CINDEX_BASEf,
                                         &entry->base_idx));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CINDEX_OPf,
                                         &entry->op_profile_id_ctr_index));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_OPf,
                                         &entry->op_profile_id_ctr_a));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_OPf,
                                         &entry->op_profile_id_ctr_b));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               inst,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Write counter action table 1 entry */
static int
bcm56990_a0_cth_ctr_eflex_action_table_1_write(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t entry_buf[ACTION_TABLE_1_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0, pipe_idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_1_INST0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1_INST0m;
    inst = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_1_INST1m :
                            FLEX_CTR_EGR_COUNTER_ACTION_TABLE_1_INST1m;

    /* Write action table 1 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 CONDITIONAL_EVICTION_OPf, &entry->evict_comp));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 CONDITIONAL_EVICTION_RESETf,
                                 &entry->evict_reset));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, CVALUE_DATAf,
                                 &entry->ctr_val_data));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, A_UPDATE_FN_TRUEf,
                                 &entry->ctr_update_mode[UPDATE_MODE_A_IDX]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, A_CVALUE_1_SELECTf,
                                 &entry->ctr_src_a));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, B_UPDATE_FN_TRUEf,
                                 &entry->ctr_update_mode[UPDATE_MODE_B_IDX]));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, B_CVALUE_1_SELECTf,
                                 &entry->ctr_src_b));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, COUNT_ON_HW_DROPf,
                                 &entry->count_on_hw_drop));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               inst,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Write counter action table entry */
static int
bcm56990_a0_cth_ctr_eflex_action_table_write(
    int unit,
    ctr_eflex_action_profile_data_t *entry)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Write ACTION_TABLE_0 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_cth_ctr_eflex_action_table_0_write(unit, entry));

    /* Write ACTION_TABLE_1 entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_cth_ctr_eflex_action_table_1_write(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

/*! Write operand profile table entry */
static int
bcm56990_a0_cth_ctr_eflex_op_profile_write(
    int unit,
    ctr_eflex_op_profile_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t entry_buf[OP_PROFILE_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0, pipe_idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLE_INST0m :
                           FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLE_INST0m;
    inst = entry->ingress ? FLEX_CTR_ING_COUNTER_OP_PROFILE_TABLE_INST1m :
                            FLEX_CTR_EGR_COUNTER_OP_PROFILE_TABLE_INST1m;

    /* Write operand profile table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        pt_dyn_info.index = entry->op_profile_index;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SEL_1f, &entry->obj_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SEL_2f, &entry->obj_2));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MASK_SIZE_1f, &entry->mask_size_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MASK_SIZE_2f, &entry->mask_size_2));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SHIFT_1f, &entry->shift_1));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 SHIFT_2f, &entry->shift_2));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               inst,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Write range check profile table entry */
static int
bcm56990_a0_cth_ctr_eflex_range_chk_write(
    int unit,
    ctr_eflex_range_chk_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t entry_buf[RANGE_CHECK_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0, pipe_idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ?
          FLEX_CTR_ING_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIG_INST0m :
          FLEX_CTR_EGR_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIG_INST0m;
    inst = entry->ingress ?
           FLEX_CTR_ING_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIG_INST1m :
           FLEX_CTR_EGR_OBJECT_QUANTIZATION_RANGE_CHECK_CONFIG_INST1m;

    /* Write range check table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        pt_dyn_info.index = entry->bank_num;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MAXf, &entry->max));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf,
                                 MINf, &entry->min));
        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               inst,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Write counter trigger entry */
static int
bcm56990_a0_cth_ctr_eflex_trigger_write(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t inst = 0;
    uint32_t entry_buf[ACTION_TABLE_0_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0, pipe_idx;
    uint32_t *data_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST0m;
    inst = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST1m :
                            FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST1m;

    /* Write action table 0 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        pt_dyn_info.index = entry->action_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /*
         * Read current PT entry.
         * Part of ACTION_TABLE_0 entry can be written by
         * CTR_xxx_EFLEX_ACTION_PROFILE LT
         */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              entry->req_flags,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              (ACTION_TABLE_0_PT_WSIZE * sizeof(uint32_t)),
                              req_ltid,
                              entry_buf,
                              &rsp_ltid,
                              &rsp_flags));

        /* Modify PT entry fields */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, TRIGGER_ENABLEf,
                                         &entry->trigger));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, START_TRIGGER_TYPEf,
                                         &entry->start));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, STOP_TRIGGER_TYPEf,
                                         &entry->stop));

        if (entry->start == TIME) {
            data_ptr = (uint32_t *)(&(entry->start_time));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_START_TIMEf, data_ptr));
        } else if (entry->start == START_CONDITION) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_START_VALUEf, &entry->start_value));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));
        }

        if (entry->stop == PERIOD) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, PERIOD_MAXf,
                                            &entry->num_periods));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf, SCALEf,
                                             &entry->scale));
        } else if (entry->stop == STOP_CONDITION) {
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_STOP_VALUEf, &entry->stop_value));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry_buf,
                    TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));
        }

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, INTERVAL_MASK_SIZEf,
                                         &entry->interval_shift));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, sid, entry_buf, INTERVAL_SIZEf,
                                         &entry->interval_size));

        /* Write PT entry */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               entry->req_flags,
                               inst,
                               &pt_dyn_info,
                               NULL,
                               entry_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Read counter trigger entry */
static int
bcm56990_a0_cth_ctr_eflex_trigger_read(
    int unit,
    ctr_eflex_trigger_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid, req_ltid = 0, rsp_ltid = 0;
    uint32_t entry_buf[ACTION_TABLE_0_PT_WSIZE] = {0};
    uint32_t rsp_flags = 0;
    uint64_t tmp_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Select correct table based on ingress value */
    sid = entry->ingress ? FLEX_CTR_ING_COUNTER_ACTION_TABLE_0_INST0m :
                           FLEX_CTR_EGR_COUNTER_ACTION_TABLE_0_INST0m;

    /* Read action table 0 for given pipe */
    pt_dyn_info.index = entry->action_id;
    pt_dyn_info.tbl_inst = entry->pipe_idx;

    /* Read current PT entry */
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          entry->req_flags,
                          sid,
                          &pt_dyn_info,
                          NULL,
                          (ACTION_TABLE_0_PT_WSIZE * sizeof(uint32_t)),
                          req_ltid,
                          entry_buf,
                          &rsp_ltid,
                          &rsp_flags));

    /* Parse PT entry fields */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, TRIGGER_ENABLEf,
                                     &entry->trigger));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, START_TRIGGER_TYPEf,
                                     &entry->start));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, STOP_TRIGGER_TYPEf,
                                     &entry->stop));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_START_TIMEf, (uint32_t *)(&tmp_data)));
    entry->start_time = tmp_data;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_START_VALUEf, &entry->start_value));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, PERIOD_MAXf,
                                    &entry->num_periods));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, SCALEf,
                                     &entry->scale));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_STOP_VALUEf, &entry->stop_value));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf,
            TRIGGER_DATA_CONDITION_MASKf, &entry->cond_mask));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, INTERVAL_MASK_SIZEf,
                                     &entry->interval_shift));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, sid, entry_buf, INTERVAL_SIZEf,
                                     &entry->interval_size));

exit:
    SHR_FUNC_EXIT();
}

/*! Get object id */
static int
bcm56990_a0_cth_ctr_eflex_object_id_get(int unit, int sel, uint64_t data)
{
    return sel ? egr_obj_fmt_tbl[data].id :
                 ing_obj_fmt_tbl[data].id;
}

/*! Get object index */
static int
bcm56990_a0_cth_ctr_eflex_object_idx_get(int unit, int sel, int data)
{
    int obj_sz;
    int val, rt;

    obj_sz = sel ? COUNTOF(egr_obj_fmt_tbl) :
                   COUNTOF(ing_obj_fmt_tbl);

    for (rt=0; rt < obj_sz; rt++) {
        val = sel ? egr_obj_fmt_tbl[rt].id :
                    ing_obj_fmt_tbl[rt].id;
        if (val == data)
            break;
    }

    return rt;
}

/*! Get action zone info */
static int
bcm56990_a0_cth_ctr_eflex_action_zone_get(int unit, int sel, uint32_t action)
{
#define FLEX_ACTION 0
#define FIXED_ACTION 1

    if (sel) {
        if (action >= 8 && action <= 15) {
            return FLEX_ACTION;
        } else if (action <= 7) {
            return FIXED_ACTION;
        }
    } else {
        if (action >= 24 && action <= 31) {
            return FLEX_ACTION;
        } else if (action <= 23) {
            return FIXED_ACTION;
        }
    }

    return FLEX_ACTION;
}

/*! Write flex state group action entry */
static int
bcm56990_a0_cth_state_eflex_group_action_write(
    int unit,
    ctr_eflex_group_action_data_t *entry)
{
    bcmdrd_sid_t num_inst[NUM_PIPE_INST];
    bcmdrd_sid_t bmp_inst[NUM_PIPE_INST];
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *grp_action_reg_buf = NULL, *grp_action_bmp_reg_buf = NULL;
    uint32_t pipe_idx;
    size_t grp_action_size, grp_action_bmp_size;
    uint32_t inst_idx, num_insts;
    size_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    num_insts = bcm56990_a0_device_info.num_pipe_instance;

    /* Select correct register based on entry ingress value */
    num_inst[0] = entry->ingress ?
                      ing_group_action_num_inst[entry->group_id] :
                      egr_group_action_num_inst[entry->group_id];

    num_inst[1] = entry->ingress ?
                      ing_group_action_num_inst[entry->group_id + NUM_ING_GROUP_ACTION] :
                      egr_group_action_num_inst[entry->group_id + NUM_EGR_GROUP_ACTION];

    bmp_inst[0] = entry->ingress ?
                      ing_group_action_bitmap_inst[entry->group_id] :
                      egr_group_action_bitmap_inst[entry->group_id];

    bmp_inst[1] = entry->ingress ?
                      ing_group_action_bitmap_inst[entry->group_id + NUM_ING_GROUP_ACTION] :
                      egr_group_action_bitmap_inst[entry->group_id + NUM_EGR_GROUP_ACTION];

    grp_action_size = bcmdrd_pt_entry_size(unit, num_inst[0]);
    grp_action_bmp_size = bcmdrd_pt_entry_size(unit, bmp_inst[0]);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(grp_action_reg_buf,
              grp_action_size,
              "bcmCthCtrEflexGrpRegBuf");
    SHR_NULL_CHECK(grp_action_reg_buf, SHR_E_MEMORY);
    sal_memset(grp_action_reg_buf, 0, grp_action_size);

    SHR_ALLOC(grp_action_bmp_reg_buf,
              grp_action_bmp_size,
              "bcmCthCtrEflexGrpBmpRegBuf");
    SHR_NULL_CHECK(grp_action_bmp_reg_buf, SHR_E_MEMORY);
    sal_memset(grp_action_bmp_reg_buf, 0, grp_action_bmp_size);

    /* Set group action registers for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, num_inst[0], pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Write counter values to all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Modify PT entry field */
            sal_memset(grp_action_reg_buf, 0, grp_action_size);
            *grp_action_reg_buf = entry->action_id;

            /* Write FLEX_CTR_XXX_GROUP_ACTION_Xr register */
            SHR_IF_ERR_EXIT
                (bcmbd_pt_write(unit,
                                num_inst[inst_idx],
                                &pt_dyn_info,
                                NULL,
                                grp_action_reg_buf));

            /* Write FLEX_CTR_XXX_GROUP_ACTION_BITMAP_Xr register */
            sal_memset(grp_action_bmp_reg_buf, 0, grp_action_bmp_size);

            for (i = 0; i < entry->num_actions; i++) {
                if (entry->action_id_map[i]) {
                    /* Set the bit for specified action id */
                    SHR_BITSET(grp_action_bmp_reg_buf,
                               entry->action_id_map[i]);
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Group action member %d. \n"),
                                    entry->action_id_map[i]));
                }
            }

            /* Write the updated bitmap reg value */
            SHR_IF_ERR_EXIT
                (bcmbd_pt_write(unit,
                                bmp_inst[inst_idx],
                                &pt_dyn_info,
                                NULL,
                                grp_action_bmp_reg_buf));
        }
    } /* end FOR */

exit:
    SHR_FREE(grp_action_reg_buf);
    SHR_FREE(grp_action_bmp_reg_buf);
    SHR_FUNC_EXIT();
}

/*! Write error stats entry */
static int
bcm56990_a0_cth_ctr_eflex_error_stats_write(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t sid_inst[2] = {0};
    uint32_t entry_sz = 0;
    uint32_t *pt_entry_buff = NULL;
    uint32_t rsp_flags = 0;
    uint32_t inst_idx, num_insts;
    uint32_t pipe_idx;
    uint32_t *data_ptr[2];
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    num_insts = bcm56990_a0_device_info.num_pipe_instance;

    /* Select counter register from common validation */
    sid = entry->ingress ? FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST0r :
                           FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST0r;

    /* Entry size in words */
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

    /* Allocate PT entry buffer */

    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthCtrEflexErrorStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));

    /* Write action table 0 for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        if (entry->upd_action_misconfig) {
            sid_inst[0] = entry->ingress ?
                        FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST0r :
                        FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST0r;
            sid_inst[1] = entry->ingress ?
                        FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST1r :
                        FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST1r;

            sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
            pt_dyn_info.index = 0;
            pt_dyn_info.tbl_inst = pipe_idx;

            /* Write counter values to all instances in pipe */
            for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
                sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
                /* Modify PT entry field */
                if (entry->start_pipe_idx == (entry->end_pipe_idx - 1)) {
                    data_ptr[inst_idx] = (inst_idx == 0) ?
                        (uint32_t *)&(entry->action_misconfig) :
                        (uint32_t *)&data;
                } else {
                    data_ptr[inst_idx] = ((pipe_idx == 0) && (inst_idx == 0)) ?
                        (uint32_t *)&(entry->action_misconfig) :
                        (uint32_t *)&data;
                }

                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         sid_inst[inst_idx],
                                         pt_entry_buff,
                                         COUNTf,
                                         data_ptr[inst_idx]));
                /* Write PT entry */
                SHR_IF_ERR_EXIT
                    (bcmptm_ireq_write(unit,
                                       0,
                                       sid_inst[inst_idx],
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       req_ltid,
                                       &rsp_ltid,
                                       &rsp_flags));
            }
        }

        if (entry->upd_too_many_actions) {
            sid_inst[0] = entry->ingress ?
                        FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTER_INST0r :
                        FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTER_INST0r;
            sid_inst[1] = entry->ingress ?
                        FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTER_INST1r :
                        FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTER_INST1r;

            sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
            pt_dyn_info.index = 0;
            pt_dyn_info.tbl_inst = pipe_idx;

            /* Write counter values to all instances in pipe */
            for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
                sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
                /* Modify PT entry field */
                if (entry->start_pipe_idx == (entry->end_pipe_idx - 1)) {
                    data_ptr[inst_idx] = (inst_idx == 0) ?
                        (uint32_t *)&(entry->too_many_actions) :
                        (uint32_t *)&data;
                } else {
                    data_ptr[inst_idx] = ((pipe_idx == 0) && (inst_idx == 0)) ?
                        (uint32_t *)&(entry->too_many_actions) :
                        (uint32_t *)&data;
                }

                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_set(unit,
                                         sid_inst[inst_idx],
                                         pt_entry_buff,
                                         COUNTf,
                                         data_ptr[inst_idx]));
                /* Write PT entry */
                SHR_IF_ERR_EXIT
                    (bcmptm_ireq_write(unit,
                                       0,
                                       sid_inst[inst_idx],
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       req_ltid,
                                       &rsp_ltid,
                                       &rsp_flags));
            }
        }
    } /* end FOR pipe */

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

/*! Read error stats entry */
static int
bcm56990_a0_cth_ctr_eflex_error_stats_read(
    int unit,
    ctr_eflex_error_stats_data_t *entry)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t sid = 0, req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t sid_inst[2] = {0};
    uint32_t entry_sz = 0;
    uint32_t *pt_entry_buff = NULL;
    uint32_t rsp_flags = 0, rsp_entry_wsize = 0;
    uint32_t inst_idx, num_insts;
    uint32_t pipe_idx;
    uint64_t tmp_data;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    num_insts = bcm56990_a0_device_info.num_pipe_instance;

    /* Select counter register from common validation */
    sid = entry->ingress ? FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST0r :
                           FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST0r;

    /* Entry size in words */
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    rsp_entry_wsize = entry_sz;

    /* Allocate PT entry buffer */
    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthCtrEflexErrorStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));

    /* Read error stats from all applicable pipes */
    entry->action_misconfig = 0;
    entry->too_many_actions = 0;
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {
        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid, pipe_idx, -1)) {
            continue;
        }

        /* Read ACTION_MISCONFIG start */
        sid_inst[0] = entry->ingress ?
                    FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST0r :
                    FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST0r;
        sid_inst[1] = entry->ingress ?
                    FLEX_CTR_ING_FC_ACTION_MISCONFIG_COUNTER_INST1r :
                    FLEX_CTR_EGR_FC_ACTION_MISCONFIG_COUNTER_INST1r;

        sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Read counter values from all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ireq_read(unit,
                                                     0,
                                                     sid_inst[inst_idx],
                                                     &pt_dyn_info,
                                                     NULL,
                                                     rsp_entry_wsize,
                                                     req_ltid,
                                                     pt_entry_buff,
                                                     &rsp_ltid,
                                                     &rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit, sid_inst[inst_idx],
                                      pt_entry_buff, COUNTf,
                                      (uint32_t *)(&tmp_data)));
            entry->action_misconfig += tmp_data;
        }
        /* Read ACTION_MISCONFIG end */

        /* Read TOO_MANY_ACTIONS start */
        sid_inst[0] = entry->ingress ?
                    FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTER_INST0r :
                    FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTER_INST0r;
        sid_inst[1] = entry->ingress ?
                    FLEX_CTR_ING_FC_TOO_MANY_ACTIONS_COUNTER_INST1r :
                    FLEX_CTR_EGR_FC_TOO_MANY_ACTIONS_COUNTER_INST1r;

        sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Read counter values from all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ireq_read(unit,
                                                     0,
                                                     sid_inst[inst_idx],
                                                     &pt_dyn_info,
                                                     NULL,
                                                     rsp_entry_wsize,
                                                     req_ltid,
                                                     pt_entry_buff,
                                                     &rsp_ltid,
                                                     &rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_get(unit, sid_inst[inst_idx],
                                      pt_entry_buff, COUNTf,
                                      (uint32_t *)(&tmp_data)));
            entry->too_many_actions += tmp_data;
        }
        /* Read TOO_MANY_ACTIONS end */
    }/* End for pipe */

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_ctr_eflex_pipe_valid_check(int unit,
                                           bool ingress,
                                           uint32_t pipe,
                                           bool *valid)
{
    uint32_t valid_pipe_map;
    int blktype;
    const bcmdrd_chip_info_t *cinfo = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get blktype */
    cinfo = bcmdrd_dev_chip_info_get(unit);

    blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "flex_ctr");

    /* Get valid pipe bitmap */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit,
                                        0,
                                        blktype,
                                        &valid_pipe_map));

    *valid = (valid_pipe_map & (1 << pipe));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get list of UPDATE_CONTROLr and fields.
 */
static int
bcm56990_a0_cth_ctr_eflex_update_control_get(
        int unit,
        bool ingress,
        bcmdrd_sid_t **update_control_list,
        uint32_t *num,
        bcmdrd_fid_t *counter_pool_enable_fid)
{

    SHR_FUNC_ENTER(unit);

    if (ingress) {
        *update_control_list = ingress_update_control_reg;
        *num = COUNTOF(ingress_update_control_reg);
    }
    else {
        *update_control_list = egress_update_control_reg;
        *num = COUNTOF(egress_update_control_reg);
    }

    *counter_pool_enable_fid = COUNTER_POOL_ENABLEf;

    SHR_FUNC_EXIT();
}

/*! Write flex state range check profile table entry */
static int
bcm56990_a0_cth_ctr_eflex_obj_quant_write(int unit,
                                          ctr_eflex_obj_quant_data_t *entry)
{
    bcmdrd_sid_t sid[NUM_PIPE_INST], req_ltid = 0, rsp_ltid = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t *entry_buf = NULL;
    uint32_t rsp_flags = 0, pipe_idx;
    size_t wsize;
    uint32_t inst_idx, num_insts;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    num_insts = bcm56990_a0_device_info.num_pipe_instance;

    /* Select correct register based on entry ingress value */
    sid[0] = entry->ingress ? FLEX_CTR_ING_OBJECT_QUANTIZATION_CTRL_INST0r :
                              FLEX_CTR_EGR_OBJECT_QUANTIZATION_CTRL_INST0r;
    sid[1] = entry->ingress ? FLEX_CTR_ING_OBJECT_QUANTIZATION_CTRL_INST1r :
                              FLEX_CTR_EGR_OBJECT_QUANTIZATION_CTRL_INST1r;

    wsize = bcmdrd_pt_entry_wsize(unit, sid[0]);

    /* Allocate reg buffer and clear */
    SHR_ALLOC(entry_buf,
             (wsize * sizeof(uint32_t)),
             "bcmCthCtrEflexObjQuantBuf");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, (wsize * sizeof(uint32_t)));

    /* Write range check table for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Ignore disabled pipes */
        if (!bcmdrd_pt_index_valid(unit, sid[0], pipe_idx, -1)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Skipping disabled pipe %d. \n"), pipe_idx));
            continue;
        }

        pt_dyn_info.index = 0;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Write counter values to all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Modify PT entry fields */
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_0_ENABLEf, &entry->quantize[0]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_1_ENABLEf, &entry->quantize[1]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_2_ENABLEf, &entry->quantize[2]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_3_ENABLEf, &entry->quantize[3]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_0_OBJECTf, &entry->obj[0]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_1_OBJECTf, &entry->obj[1]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_2_OBJECTf, &entry->obj[2]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_3_OBJECTf, &entry->obj[3]));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_0_1_MODEf, &entry->obj_0_1_mode));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid[inst_idx], entry_buf,
                                     BANK_2_3_MODEf, &entry->obj_2_3_mode));

            /* Write PT entry */
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write(unit,
                                   entry->req_flags,
                                   sid[inst_idx],
                                   &pt_dyn_info,
                                   NULL,
                                   entry_buf,
                                   req_ltid,
                                   &rsp_ltid,
                                   &rsp_flags));
        } /* end FOR all instances */
    } /* end FOR pipe */

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * Function to check if OBJ_QUANTIZATION handlers should be
 * registered with IMM based CTH.
 */
static int
bcm56990_a0_cth_ctr_eflex_obj_quant_imm_reg(int unit, bool *imm_reg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(imm_reg, SHR_E_PARAM);

    *imm_reg = true;

exit:
    SHR_FUNC_EXIT();
}

static bcmcth_ctr_eflex_drv_t bcm56990_a0_cth_ctr_eflex_drv = {
    .device_info_get_fn = bcm56990_a0_cth_ctr_eflex_device_info_get,
    .ctr_table_get_fn   = bcm56990_a0_cth_ctr_eflex_ctr_table_get,
    .action_reg_set_fn  = bcm56990_a0_cth_ctr_eflex_action_reg_set,
    .action_table_write_fn = bcm56990_a0_cth_ctr_eflex_action_table_write,
    .op_profile_write_fn = bcm56990_a0_cth_ctr_eflex_op_profile_write,
    .range_chk_write_fn = bcm56990_a0_cth_ctr_eflex_range_chk_write,
    .trigger_write_fn = bcm56990_a0_cth_ctr_eflex_trigger_write,
    .trigger_read_fn = bcm56990_a0_cth_ctr_eflex_trigger_read,
    .object_id_get_fn = bcm56990_a0_cth_ctr_eflex_object_id_get,
    .object_idx_get_fn = bcm56990_a0_cth_ctr_eflex_object_idx_get,
    .action_zone_get_fn = bcm56990_a0_cth_ctr_eflex_action_zone_get,
    .group_action_write_fn = bcm56990_a0_cth_state_eflex_group_action_write,
    .error_stats_write_fn = bcm56990_a0_cth_ctr_eflex_error_stats_write,
    .error_stats_read_fn = bcm56990_a0_cth_ctr_eflex_error_stats_read,
    .pipe_valid_check_fn = bcm56990_a0_cth_ctr_eflex_pipe_valid_check,
    .update_control_get_fn = bcm56990_a0_cth_ctr_eflex_update_control_get,
    .obj_quant_write_fn = bcm56990_a0_cth_ctr_eflex_obj_quant_write,
    .obj_quant_imm_reg_fn = bcm56990_a0_cth_ctr_eflex_obj_quant_imm_reg
};

/*******************************************************************************
* Public functions
 */

bcmcth_ctr_eflex_drv_t *
bcm56990_a0_cth_ctr_eflex_drv_get(int unit)
{
    return &bcm56990_a0_cth_ctr_eflex_drv;
}

bcmcth_state_eflex_drv_t *
bcm56990_a0_cth_state_eflex_drv_get(int unit)
{
    return NULL;
}
