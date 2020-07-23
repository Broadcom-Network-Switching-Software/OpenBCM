/*! \file bcm56990_a0_testutil_drv.c
 *
 * Chip-specific driver utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>

#include <bcmbd/bcmbd_cmicx.h>

#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_cmicx.h>
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/chip/bcm56990_a0_testutil_traffic.h>
#include <bcmltd/bcmltd_lt_types.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define CMC_NUM 2
#define CMC_CHAN_NUM_PER_CMC 8

#define NUM_PP 4
#define NUM_PORT_PER_PP 68
#define AT_DUPLICATE 17
#define AT_DUPLICATE2 23

#define L2_BANK_ENTRIES (4 * 1024)
#define EXACT_MATCH_BANK_ENTRIES (32 * 1024)
#define MPLS_BANK_ENTRIES (4 * 1024)
#define L3_TUNNEL_BANK_ENTRIES (4 * 1024)
#define EGR_ADAPT_BANK_ENTRIES (4 * 1024)

#define CDMIB_MEM_FMT "MIB_MEMORY_ROW"

static bcmdrd_sid_t bcm56990_a0_reg_reset_skip_pt[] = {
    /* These registers are modified in device init phase. */
    MMU_GLBCFG_MISCCONFIGr,
    CDMAC_MIB_COUNTER_CTRLr,
    CDPORT_SBUS_CONTROLr,
    EGR_DII_NULL_SLOT_CFGr,
    ING_DII_NULL_SLOT_CFGr,

    /* These registers should be added NOTEST flag. */
    AVS_TMON_SPARE_0r,

    /* These registers are half chip related. */
    AVS_PMB_SLAVE_AVS_ROSC_LVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_LVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT8_THRESHOLDr,
};

static bcmdrd_sid_t bcm56990_a0_reg_wr_skip_pt[] = {
    /* These registers should be added NOTEST flag. */
    AVS_TMON_SPARE_0r,
    MMU_RQE_QUEUE_SNAPSHOT_ENr,

    /* These registers are half chip related. */
    AVS_PMB_SLAVE_AVS_ROSC_LVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_LVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT8_THRESHOLDr,
};

static bcmdrd_sid_t bcm56990_a0_mem_skip_pt[] = {
    /* These tables should be added NOTEST flag. */
    ETRAP_SAMPLE_FLOW_COUNT_LEFT_INST0m,
    ETRAP_SAMPLE_FLOW_COUNT_LEFT_INST1m,
    ETRAP_SAMPLE_FLOW_COUNT_RIGHT_INST0m,
    ETRAP_SAMPLE_FLOW_COUNT_RIGHT_INST1m,
    ETRAP_SAMPLE_FLOW_CTRL_LEFT_INST0m,
    ETRAP_SAMPLE_FLOW_CTRL_LEFT_INST1m,
    ETRAP_SAMPLE_FLOW_CTRL_RIGHT_INST0m,
    ETRAP_SAMPLE_FLOW_CTRL_RIGHT_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_L0_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_L0_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_L1_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_L1_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_L2_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_L2_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_L3_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_L3_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_L4_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_L4_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_R0_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_R0_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_R1_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_R1_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_R2_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_R2_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_R3_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_R3_INST1m,
    ETRAP_SAMPLE_FLOW_HASH_R4_INST0m,
    ETRAP_SAMPLE_FLOW_HASH_R4_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_INST1m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_INST0m,
    FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_INST1m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_INST0m,
    FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_INST1m,
};

static bcmdrd_sid_t bcm56990_a0_mem_reset_skip_pt[] = {
    /* This memory is modified in device init phase. */
    PORT_TABm,
    EGR_PORTm,
    MMU_EBCFP_MXM_TAG_MEMm,

    
    EXACT_MATCH_HIT_ONLYm,
    EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm,
    IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM3_IOM_STATS_WINDOW_RESULTSm,
    MGMT_OBM_IOM_STATS_WINDOW_RESULTSm,
    MMU_EBTOQ_CFPm,
    MMU_EBTOQ_QDBm,
    MMU_EBCFP_FAP_BITMAP_MEMm,
    MMU_EBMB_CCBE_SLICEm,
    MMU_RQE_CELL_FREE_LISTm,
    MMU_RQE_PKTQ_FREE_LISTm,
    MMU_RQE_PKT_STATEm,
    MMU_SCB_PIPE_FAPm,
    MMU_TOQ_MC_CQEB_FAP_MMU_ITM0_ITM0m,
    MMU_TOQ_MC_CQEB_FAP_MMU_ITM1_ITM1m,
    MMU_TOQ_UC_CQEB_FAP_MMU_ITM0_ITM0m,
    MMU_TOQ_UC_CQEB_FAP_MMU_ITM1_ITM1m,
};

static shr_enum_map_t bcm56990_a0_blktype_name_id_map[] = {
    {"avs", BLKTYPE_AVS},
    {"cdport", BLKTYPE_CDPORT},
    {"cmic", BLKTYPE_CMIC},
    {"epipe", BLKTYPE_EPIPE},
    {"flex_ctr", BLKTYPE_FLEX_CTR},
    {"ipipe", BLKTYPE_IPIPE},
    {"iproc", BLKTYPE_IPROC},
    {"lbport", BLKTYPE_LBPORT},
    {"mgmt_obm", BLKTYPE_MGMT_OBM},
    {"mmu_eb", BLKTYPE_MMU_EB},
    {"mmu_glb", BLKTYPE_MMU_GLB},
    {"mmu_itm", BLKTYPE_MMU_ITM},
    {"pfc_collector", BLKTYPE_PFC_COLLECTOR},
    {"port_if", BLKTYPE_PORT_IF},
    {"ser", BLKTYPE_SER},
    {"top", BLKTYPE_TOP},
    {"xlport", BLKTYPE_XLPORT}
};

static bcmdrd_sid_t bcm56990_a0_tr144_skip_pt_list[] = {
    /* NACK isn't set after triggering SER error */
    IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM3_IOM_STATS_WINDOW_RESULTSm,
    /* Cannot trigger SER error */
    MGMT_OBM_IOM_STATS_WINDOW_RESULTSm,

    EGR_PRI_CNG_MAPm,
    STG_TAB_Am,
    STG_TAB_Bm
};

static bcmdrd_sid_t bcm56990_a0_tr901_skip_pt_list[] = {
    MPLS_ENTRY_DOUBLEm
};

static const bcma_testutil_pkt_lt_cfg_t bcm56990_a0_pkt_traffic_lt_cfg[] = {
    {"DEVICE_PKT_RX_Q", {{"RX_Q", 1}, {"COS", 1, NULL, 48, 0}}, BCMLT_OPCODE_INSERT},
    {"PC_PORT", {{"PORT_ID", 36}, {"LOOPBACK_MODE", 0, "PC_LPBK_MAC"}}, BCMLT_OPCODE_UPDATE},
    {"PC_PORT", {{"PORT_ID", 2}, {"LOOPBACK_MODE", 0, "PC_LPBK_MAC"}}, BCMLT_OPCODE_UPDATE},
    {"LM_PORT_CONTROL", {{"PORT_ID", 36}}, BCMLT_OPCODE_INSERT},
    {"LM_PORT_CONTROL", {{"PORT_ID", 2}}, BCMLT_OPCODE_INSERT},
    {"LM_LINK_STATE", {{"PORT_ID", 36}}, BCMLT_OPCODE_LOOKUP},
    {"LM_LINK_STATE", {{"PORT_ID", 2}}, BCMLT_OPCODE_LOOKUP},
    {"VLAN_STG", {{"VLAN_STG_ID", 1}, {"STATE", 0, "FORWARD", 3, 1}}, BCMLT_OPCODE_INSERT},
    {"VLAN_STG", {{"VLAN_STG_ID", 1}, {"STATE", 0, "FORWARD", 37, 36}}, BCMLT_OPCODE_UPDATE},
    {"VLAN_ING_TAG_ACTION_PROFILE",
     {{"VLAN_ING_TAG_ACTION_PROFILE_ID", 1}, {"UT_OTAG", 0, "ADD"}}, BCMLT_OPCODE_INSERT},
    {"VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
     {{"VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 1},{"EGR_MEMBER_PORTS", 1, NULL, 3, 1}},
     BCMLT_OPCODE_INSERT},
    {"VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
     {{"VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 1},{"EGR_MEMBER_PORTS", 1, NULL, 37, 36}},
     BCMLT_OPCODE_UPDATE},
    {"VLAN",
     {{"VLAN_ID", 2}, {"EGR_MEMBER_PORTS", 1, NULL, 3, 1}, {"ING_MEMBER_PORTS", 1, NULL, 3, 1},
      {"UNTAGGED_MEMBER_PORTS", 1, NULL, 3, 1}, {"VLAN_STG_ID", 1}, {"VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 1}},
     BCMLT_OPCODE_INSERT},
    {"VLAN",
     {{"VLAN_ID", 2}, {"EGR_MEMBER_PORTS", 1, NULL, 37, 36}, {"ING_MEMBER_PORTS", 1, NULL, 37, 36},
      {"UNTAGGED_MEMBER_PORTS", 1, NULL, 37, 36}}, BCMLT_OPCODE_UPDATE},
    {"PORT",
     {{"PORT_ID", 2}, {"PORT_TYPE", 0, "ETHERNET"}, {"ING_OVID", 0x2}, {"VLAN_ING_TAG_ACTION_PROFILE_ID", 1}},
     BCMLT_OPCODE_INSERT},
    {"PORT",
     {{"PORT_ID", 36}, {"PORT_TYPE", 0, "ETHERNET"}, {"ING_OVID", 0x2}, {"VLAN_ING_TAG_ACTION_PROFILE_ID", 1}},
     BCMLT_OPCODE_INSERT},
    {"L2_FDB_VLAN_STATIC",
     {{"VLAN_ID", 2}, {"VLAN_ID_MASK", 0xfff}, {"MAC_ADDR", 0x000000BBBBBB},
      {"MAC_ADDR_MASK", 0xFFFFFFFFFFFF}, {"MODPORT", 36}, {"COPY_TO_CPU", 1}},
     BCMLT_OPCODE_INSERT},
};

static uint8_t bcm56990_a0_pkt_raw_data[] = {
    0x00,0x00,0x00,0xbb,0xbb,0xbb,0x00,0x00,
    0x00,0x00,0x00,0x01,0x81,0x00,0x00,0x02,
    0xff,0xff,0x01,0x02,0x03,0x04,0x05,0x06,
    0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
    0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
    0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,
    0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,
    0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static uint32_t bcm56990_a0_tx_pmd[][3] = {
    {0x81000000, 0x1, 0x4000}
};

bcmdrd_ser_rmr_id_t bcm56990_a0_bb_skip_pt[8] = {0};

static const bcma_testutil_pkt_data_t bcm56990_a0_pkt_data = {
    bcm56990_a0_tx_pmd,
    sizeof(bcm56990_a0_tx_pmd),
    NULL, 0, NULL, 0,
    bcm56990_a0_pkt_raw_data,
    sizeof(bcm56990_a0_pkt_raw_data)
};

static const uint32_t default_all_zero[BCMDRD_MAX_PT_WSIZE] = {0x0};

static bool
bcm56990_a0_pt_skip(int unit, bcmdrd_sid_t sid, bcma_testutil_pt_type_t type)
{
    bcmdrd_sid_t *skip_pt;
    int blktype = 0, acctype = 0, count = 0, idx;
    uint32_t mmu_itm_map;
    bool emul;
    const char *pt_name;

    switch (type) {
    case BCMA_TESTUTIL_PT_REG_RESET:
        skip_pt = bcm56990_a0_reg_reset_skip_pt;
        count = COUNTOF(bcm56990_a0_reg_reset_skip_pt);
        break;

    case BCMA_TESTUTIL_PT_REG_RD_WR:
        skip_pt = bcm56990_a0_reg_wr_skip_pt;
        count = COUNTOF(bcm56990_a0_reg_wr_skip_pt);
        break;

    case BCMA_TESTUTIL_PT_MEM_RD_WR:
    case BCMA_TESTUTIL_PT_MEM_DMA_RD_WR:
        /* Memory test only on EP, IP, MMU, SER */
        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        switch (blktype) {
        case BLKTYPE_EPIPE:
        case BLKTYPE_IPIPE:
        case BLKTYPE_MMU_GLB:
        case BLKTYPE_MMU_ITM:
        case BLKTYPE_MMU_EB:
        case BLKTYPE_SER:
        case BLKTYPE_PORT_IF:
        case BLKTYPE_FLEX_CTR:
        case BLKTYPE_MGMT_OBM:
        case BLKTYPE_PFC_COLLECTOR:
            skip_pt = bcm56990_a0_mem_skip_pt;
            count = COUNTOF(bcm56990_a0_mem_skip_pt);
            break;
        default:
            return TRUE;
        }
        break;

    case BCMA_TESTUTIL_PT_MEM_RESET:
        
        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        if (sal_strstr(pt_name, "MMU_CFAP_BANK") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_MB_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB0_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB1_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB2_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB3_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB4_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB5_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB6_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_EBMB7_PAYLOAD_SLICE") != NULL) {
            return TRUE;
        }

        blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        switch (blktype) {
        case BLKTYPE_EPIPE:
        case BLKTYPE_IPIPE:
        case BLKTYPE_MMU_GLB:
        case BLKTYPE_MMU_ITM:
        case BLKTYPE_MMU_EB:
        case BLKTYPE_SER:
        case BLKTYPE_PORT_IF:
        case BLKTYPE_FLEX_CTR:
        case BLKTYPE_MGMT_OBM:
        case BLKTYPE_PFC_COLLECTOR:
            skip_pt = bcm56990_a0_mem_reset_skip_pt;
            count = COUNTOF(bcm56990_a0_mem_reset_skip_pt);
            break;
        default:
            return TRUE;
        }
        break;

    case BCMA_TESTUTIL_PT_MEM_TR_144:
        skip_pt = bcm56990_a0_tr144_skip_pt_list;
        count = COUNTOF(bcm56990_a0_tr144_skip_pt_list);
        break;
    case BCMA_TESTUTIL_PT_MEM_TR_143:
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DII_EVENT_FIFO_0", &bcm56990_a0_bb_skip_pt[0]);
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DII_EVENT_FIFO_1", &bcm56990_a0_bb_skip_pt[1]);
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DII_EVENT_FIFO_2", &bcm56990_a0_bb_skip_pt[2]);
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DII_EVENT_FIFO_3", &bcm56990_a0_bb_skip_pt[3]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_0", &bcm56990_a0_bb_skip_pt[4]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_1", &bcm56990_a0_bb_skip_pt[5]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_2", &bcm56990_a0_bb_skip_pt[6]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_3", &bcm56990_a0_bb_skip_pt[7]);
        skip_pt = bcm56990_a0_bb_skip_pt;
        count = COUNTOF(bcm56990_a0_bb_skip_pt);
        break;
    case BCMA_TESTUTIL_PT_MEM_TR_901:
        skip_pt = bcm56990_a0_tr901_skip_pt_list;
        count = COUNTOF(bcm56990_a0_tr901_skip_pt_list);
        break;

    default:
        return FALSE;
    }

    for (idx = 0; idx < count; idx++) {
        if (sid == skip_pt[idx]) {
            return TRUE;
        }
    }

    if (type == BCMA_TESTUTIL_PT_MEM_DMA_RD_WR) {
        
        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        if (sal_strstr(pt_name, "MMU_EBCFP_LAT_ABS_FIFO") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MMU_TOQ_VOQ_HEAD_DB_CPU") != NULL) {
            return TRUE;
        } else if (sal_strstr(pt_name, "MAC_BLOCK") != NULL) {
            return TRUE;
        }

        if (bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_MMU_ITM, &mmu_itm_map)
            != SHR_E_NONE) {
            return TRUE;
        }

        emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

        /*
         * Access MMU tables with DUPLICATE access type through SBUSDMA
         * get sbus ack with error bit set on emulator half chip mode.
         */
        if (emul && mmu_itm_map != 0x3) {
            acctype = bcmdrd_pt_acctype_get(unit, sid);
            if ((blktype == BLKTYPE_MMU_ITM || blktype == BLKTYPE_MMU_EB)
                && acctype == AT_DUPLICATE) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static void
bcm56990_a0_pkt_cfg_data_get(int unit, const bcma_testutil_pkt_lt_cfg_t **pkt_lt_cfg,
                             int *lt_cfg_num,
                             const bcma_testutil_pkt_data_t **pkt_data)
{
    *lt_cfg_num = COUNTOF(bcm56990_a0_pkt_traffic_lt_cfg);
    *pkt_lt_cfg = bcm56990_a0_pkt_traffic_lt_cfg;

    *pkt_data = &bcm56990_a0_pkt_data;
}

static int
reg_hw_update(int unit, const char *name, bool enable)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t field_data;

    SHR_FUNC_ENTER(unit);

    /* Disable hardware update to enable sw update MMU_INTFO_TO_XPORT_BKP. */
    sid = MMU_INTFO_XPORT_BKP_HW_UPDATE_DISr;
    fid = PAUSE_PFC_BKPf;
    field_data = enable ? 0 : 1;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    if (!enable) {
        /*
         * Deassert IDB control registers, otherwise some IDB status registers
         * will always read 0 even write non-zero value.
         */
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA0_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA1_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA2_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA3_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_OBM0_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_OBM1_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_OBM2_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_OBM3_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, MGMT_OBM_CONTROLr));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mem_hw_update(int unit, const char *name, bool enable)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t field_data;

    SHR_FUNC_ENTER(unit);

    /*
     * ECCP_EN is to enable hardware to generate ecc/parity bits
     * 0: prevent hardware automatically overwriting the ecc and
     *    parity field
     */
    field_data = enable ? 1 : 0;

    sid = MMU_GLBCFG_MISCCONFIGr;
    fid = ECCP_ENf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    if (sal_strcmp(name, "LINK_STATUSm") == 0 || sal_strcmp(name, "*") == 0) {
        /* Enables status updates of LINK_STATUS via the CPU */
        field_data = enable ? 0 : 1;

        sid = SW_HW_CONTROLr;
        fid = LINK_STATUS_UPDATE_ENABLEf;
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pt_hw_update(int unit, const char *name,
                         bcma_testutil_pt_type_t type, bool enable)
{
    if (type == BCMA_TESTUTIL_PT_MEM_RD_WR ||
        type == BCMA_TESTUTIL_PT_MEM_DMA_RD_WR) {
        return mem_hw_update(unit, name, enable);
    } else {
        return reg_hw_update(unit, name, enable);
    }

    return SHR_E_NONE;
}

static int
uft_bank_num_get(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_fid_t fid = HASH_TABLE_BANK_CONFIGf;
    int count = 0, minbit, maxbit, bits, idx, rv;
    uint32_t banks[1] = {0};

    minbit = bcmdrd_pt_field_minbit(unit, sid, fid);
    maxbit = bcmdrd_pt_field_maxbit(unit, sid, fid);
    if (minbit == -1 || maxbit == -1) {
        return 0;
    }

    rv = bcma_testutil_pt_field_get(unit, sid, fid, 0, 0, banks);
    if (SHR_FAILURE(rv)) {
        return 0;
    }

    bits = maxbit - minbit + 1;
    for (idx = 0; idx < bits; idx++) {
        if ((banks[0] >> idx) & 0x1) {
            count++;
        }
    }

    return count;
}

/*
 * Adjust unified forwarding table size based on the allocation of banks.
 */
static int
uft_entries_calculate(int unit, int *l2_entries, int *egr_adapt_entries,
                      int *em_entries, int *mpls_entries,
                      int *l3tnl_entries)
{
    bcmdrd_sid_t sid;
    int banks;

    /*
     * L2: 4k entries per bank
     * Exact Match: 32k entries per bank
     * MPLS: 4k entries per bank
     * L3 Tunnel: 4k entries per bank
     */

    sid = L2_ENTRY_HASH_CONTROLm;
    banks = uft_bank_num_get(unit, sid);
    *l2_entries = L2_BANK_ENTRIES * banks;

    sid = EXACT_MATCH_HASH_CONTROLm;
    banks = uft_bank_num_get(unit, sid);
    *em_entries = EXACT_MATCH_BANK_ENTRIES * banks;

    sid = MPLS_ENTRY_HASH_CONTROLm;
    banks = uft_bank_num_get(unit, sid);
    *mpls_entries = MPLS_BANK_ENTRIES * banks;

    sid = L3_TUNNEL_HASH_CONTROLm;
    banks = uft_bank_num_get(unit, sid);
    *l3tnl_entries = L3_TUNNEL_BANK_ENTRIES * banks;

    sid = EGR_ADAPT_HASH_CONTROLm;
    banks = uft_bank_num_get(unit, sid);
    *egr_adapt_entries = EGR_ADAPT_BANK_ENTRIES * banks;

    return SHR_E_NONE;
}

static int
uft_init(int unit, bcmdrd_sid_t sid)
{
    int rv = SHR_E_NONE;

    switch (sid) {
    /* L2 table */
    case L2_ENTRY_SINGLEm:
    case L2_ENTRY_ECCm:
        rv = bcma_testutil_pt_clear_all(unit, L2_ENTRY_KEY_ATTRIBUTESm);
        break;

    /* EXACT MATCH table */
    case EXACT_MATCH_2m:
    case EXACT_MATCH_4m:
    case EXACT_MATCH_ECCm:
        rv = bcma_testutil_pt_clear_all(unit, EXACT_MATCH_KEY_ATTRIBUTESm);
        break;

    /* MPLS table */
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRY_ECCm:
    case MPLS_ENTRY_DOUBLEm:
        rv = bcma_testutil_pt_clear_all(unit, MPLS_ENTRY_KEY_ATTRIBUTESm);
        break;

    /* L3 tunnel table */
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_ECCm:
    case L3_TUNNEL_DOUBLEm:
    case L3_TUNNEL_QUADm:
        rv = bcma_testutil_pt_clear_all(unit, L3_TUNNEL_KEY_ATTRIBUTESm);
        break;

    case EGR_ADAPT_SINGLEm:
    case EGR_ADAPT_ECCm:
        rv = bcma_testutil_pt_clear_all(unit, EGR_ADAPT_KEY_ATTRIBUTESm);
        break;
    }

    return rv;
}

static int
bcm56990_a0_mem_address_adjust(int unit, bcmdrd_sid_t sid,
                               int *index_start, int *index_end,
                               bcma_testutil_pt_type_t type)
{
    static int inited = 0;
    static int l2_entries = 0;
    static int egr_adapt_entries = 0;
    static int em_entries = 0;
    static int mpls_entries = 0;
    static int l3tnl_entries = 0;

    SHR_FUNC_ENTER(unit);

    if (!inited) {
        SHR_IF_ERR_EXIT
            (uft_entries_calculate(unit, &l2_entries, &egr_adapt_entries,
                                   &em_entries, &mpls_entries,
                                   &l3tnl_entries));
        inited = 1;
    }

    /*
     * UFT attribute table could be test before and some field could be changed
     * to invalid value, for example: KEY_BASE_WIDTH.
     * That will affect UFT table test.
     */
    SHR_IF_ERR_EXIT
        (uft_init(unit, sid));

    switch (sid) {
    /* L2 table */
    case L2_ENTRY_SINGLEm:
    case L2_ENTRY_ECCm:
        *index_end = l2_entries - 1;
        break;

    /* EXACT MATCH table */
    case EXACT_MATCH_2m:
        *index_end = em_entries / 2 - 1;
        break;
    case EXACT_MATCH_4m:
        *index_end = em_entries / 4 - 1;
        break;
    case EXACT_MATCH_ECCm:
        *index_end = em_entries - 1;
        break;

    /* MPLS table */
    case MPLS_ENTRY_SINGLEm:
    case MPLS_ENTRY_ECCm:
        *index_end = mpls_entries - 1;
        break;
    case MPLS_ENTRY_DOUBLEm:
        *index_end = mpls_entries / 2 - 1;
        break;

    /* L3 tunnel table */
    case L3_TUNNEL_SINGLEm:
    case L3_TUNNEL_ECCm:
        *index_end = l3tnl_entries - 1;
        break;
    case L3_TUNNEL_DOUBLEm:
        *index_end = l3tnl_entries / 2 - 1;
        break;
    case L3_TUNNEL_QUADm:
        *index_end = l3tnl_entries / 4 - 1;
        break;

    case EGR_ADAPT_SINGLEm:
    case EGR_ADAPT_ECCm:
        *index_end = egr_adapt_entries - 1;
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cmc_chan_get(int unit, int *cmc_num, int *cmc_chan_num)
{
    if (cmc_num != NULL) {
        *cmc_num = CMC_NUM;
    }

    if (cmc_chan_num != NULL) {
        *cmc_chan_num = CMC_CHAN_NUM_PER_CMC;
    }

    return SHR_E_NONE;
}

static int
bcm56990_a0_chip_blktype_get(int unit, const char *blktype_name,
                             int *blktype_id)
{
    int count = COUNTOF(bcm56990_a0_blktype_name_id_map), block;

    block = bcma_testutil_name2id(bcm56990_a0_blktype_name_id_map,
                                  count, blktype_name);

    *blktype_id = block;

    return SHR_E_NONE;
}

static const bcmdrd_sid_t bcm56990_a0_becchmark_reg[] = {
    VLAN_CTRLr,        /* socreg */
    CMIC_RPE_PKT_CTRLr /* cmicreg */
};

static const bcmdrd_sid_t bcm56990_a0_becchmark_mem[] = {
    VLAN_TABm,        /* index */
    L2_ENTRY_SINGLEm, /* hash */
    IFP_TCAMm         /* tcam */
};

static int
bcm56990_a0_benchmark_reglist_get(int unit, const bcmdrd_sid_t **reg_arr,
                                  int *reg_cnt)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(reg_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(reg_cnt, SHR_E_PARAM);

    *reg_arr = bcm56990_a0_becchmark_reg;
    *reg_cnt = COUNTOF(bcm56990_a0_becchmark_reg);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_benchmark_memlist_get(int unit, const bcmdrd_sid_t **mem_arr,
                                  int *mem_cnt)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(mem_cnt, SHR_E_PARAM);

    *mem_arr = bcm56990_a0_becchmark_mem;
    *mem_cnt = COUNTOF(bcm56990_a0_becchmark_mem);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mem_hash_data_adjust(int unit, bcmdrd_sid_t sid,
                                 uint32_t *data, int wsize)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    switch (sid) {
        case L2_ENTRY_SINGLEm:
        {
            L2_ENTRY_SINGLEm_t *l2xm = (L2_ENTRY_SINGLEm_t *)data;
            L2_ENTRY_SINGLEm_BASE_VALIDf_SET(*l2xm, 1);
            break;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
ipep_pt_mask_override(int unit,
                      bcmdrd_sid_t sid,
                      const bcmdrd_fid_t *fid_list,
                      size_t num_fid,
                      uint32_t *mask,
                      int wsize)
{
    bcmdrd_sym_field_info_t finfo;
    uint32_t ipep_map, pp_idx = 0xf, fid_idx, bit_idx, start_idx, end_idx;
    int w, b;

    SHR_FUNC_ENTER(unit);

    /*
     * PP0: port 0 - 67
     * PP1: port 68 - 135
     * PP2: port 136 - 203
     * PP3: port 204 - 271
     */

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_IPIPE, &ipep_map));

    for (fid_idx = 0; fid_idx < num_fid; fid_idx++) {
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit, sid, fid_list[fid_idx],
                                      &finfo));

        switch (fid_list[fid_idx]) {
        /* ALLOWED_PORT_BITMAP_PROFILEm and VLAN_2_TABm */
        case ING_PORT_BITMAPf:
        /* EGR_VLANm */
        case PORT_BITMAPf:
        /* EGR_VLAN_2m */
        case UT_PORT_BITMAPf:
            for (pp_idx = 0; pp_idx < NUM_PP; pp_idx++) {
                /* Skip these bits which belong to unavailable PP. */
                if (!(ipep_map & (1 << pp_idx))) {
                    start_idx = finfo.minbit + pp_idx * NUM_PORT_PER_PP;
                    end_idx = start_idx + NUM_PORT_PER_PP - 1;

                    for (bit_idx = start_idx; bit_idx <= end_idx;
                         bit_idx++) {
                        w = bit_idx / 32;
                        b = bit_idx % 32;
                        mask[w] &= ~(1 << b);
                    }
                }
            }
            break;

        case SPLIT_DATA_P0f:
        case SPLIT_DATA_P1f:
        case SPLIT_DATA_P2f:
        case SPLIT_DATA_P3f:
        case SPLIT_DATA_P4f:
        case SPLIT_DATA_P5f:
        case SPLIT_DATA_P6f:
        case SPLIT_DATA_P7f:
        case ECCP_P0f:
        case ECCP_P1f:
        case ECCP_P2f:
        case ECCP_P3f:
        case ECCP_P4f:
        case ECCP_P5f:
        case ECCP_P6f:
        case ECCP_P7f:
            if (fid_list[fid_idx] == SPLIT_DATA_P0f ||
                fid_list[fid_idx] == SPLIT_DATA_P1f ||
                fid_list[fid_idx] == ECCP_P0f ||
                fid_list[fid_idx] == ECCP_P1f) {
                /* These fields belong to PP0. */
                pp_idx = 0;
            } else if (fid_list[fid_idx] == SPLIT_DATA_P2f ||
                       fid_list[fid_idx] == SPLIT_DATA_P3f ||
                       fid_list[fid_idx] == ECCP_P2f ||
                       fid_list[fid_idx] == ECCP_P3f) {
                /* These fields belong to PP1. */
                pp_idx = 1;
            } else if (fid_list[fid_idx] == SPLIT_DATA_P4f ||
                       fid_list[fid_idx] == SPLIT_DATA_P5f ||
                       fid_list[fid_idx] == ECCP_P4f ||
                       fid_list[fid_idx] == ECCP_P5f) {
                /* These fields belong to PP2. */
                pp_idx = 2;
            } else if (fid_list[fid_idx] == SPLIT_DATA_P6f ||
                       fid_list[fid_idx] == SPLIT_DATA_P7f ||
                       fid_list[fid_idx] == ECCP_P6f ||
                       fid_list[fid_idx] == ECCP_P7f) {
                /* These fields belong to PP3. */
                pp_idx = 3;
            }

            /* Skip these fields which belong to unavailable PP. */
            if (!(ipep_map & (1<< pp_idx))) {
                for (bit_idx = finfo.minbit; bit_idx <= finfo.maxbit;
                     bit_idx++) {
                    w = bit_idx / 32;
                    b = bit_idx % 32;
                    mask[w] &= ~(1 << b);
                }
            }
            break;

        default:
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cdport_pt_mask_override(int unit,
                        bcmdrd_sid_t sid,
                        const bcmdrd_fid_t *fid_list,
                        size_t num_fid,
                        uint32_t *mask,
                        int wsize)
{
    bcmdrd_sym_field_info_t finfo;
    uint32_t fid_idx;
    int n, w, b;

    SHR_FUNC_ENTER(unit);

    sal_memset(mask, 0, wsize * sizeof(uint32_t));
    for (fid_idx = 0; fid_idx < num_fid; fid_idx++) {
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit, sid, fid_list[fid_idx], &finfo));
        if (sal_strstr(finfo.name, "RESERVED") != NULL) {
            /* Skip reserve field. */
            continue;
        }

        for (n = finfo.minbit; n <= finfo.maxbit; n++) {
            w = n / 32;
            b = n % 32;

            mask[w] |= (1 << b);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pt_mask_override(int unit, bcmdrd_sid_t sid, int index,
                             uint32_t *mask, int wsize)
{
    char cdmib_fmt_name[20];
    bcmdrd_sid_t cdmib_fmt_sid;
    bcmdrd_fid_t *fid_list = NULL;
    size_t num_fid;
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (sid) {
    case ALLOWED_PORT_BITMAP_PROFILEm:
    case VLAN_2_TABm:
    case EGR_VLANm:
    case EGR_VLAN_2m:
    case EGR_VLAN_STG_Am:
    case STG_TAB_Am:
    case EGR_VLAN_STG_Bm:
    case STG_TAB_Bm:
        /* Get field list. */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &num_fid));

        SHR_ALLOC(fid_list, num_fid * sizeof(bcmdrd_fid_t), "bcmaTestFidList");
        SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
        sal_memset(fid_list, 0, num_fid * sizeof(bcmdrd_fid_t));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, sid, num_fid, fid_list, &num_fid));

        /* Override ipep table mask. */
        SHR_IF_ERR_EXIT
            (ipep_pt_mask_override(unit, sid, fid_list, num_fid, mask, wsize));

        break;

    case CDMIB_MEMm:
        /*
         * Each entry has 8 40-bit counters with 13 entries per port.
         * Reserve 3 entries for the future expansion per port.
         * So the actual number of entries per port is 16.
         * The format of each entry is defined in MIB_MEMORY_ROW.
         */
        idx = index % 16;
        sal_sprintf(cdmib_fmt_name, "%s%d", CDMIB_MEM_FMT, idx);
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_name_to_sid(unit, cdmib_fmt_name, &cdmib_fmt_sid));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, cdmib_fmt_sid, 0, NULL, &num_fid));

        SHR_ALLOC(fid_list, num_fid * sizeof(bcmdrd_fid_t), "bcmaTestFidList");
        SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
        sal_memset(fid_list, 0, num_fid * sizeof(bcmdrd_fid_t));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_fid_list_get(unit, cdmib_fmt_sid, num_fid, fid_list,
                                    &num_fid));

        SHR_IF_ERR_EXIT
            (cdport_pt_mask_override(unit, cdmib_fmt_sid, fid_list, num_fid,
                                     mask, wsize));

        break;

    case MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm:
        /*
         * Read one MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm entry
         * will return four consective MMU_THDO_BST_SHARED_PORTSP_MCm entries.
         * MMU_THDO_BST_SHARED_PORTSP_MCm mask is 28 bits.
         */
        for (idx = 0; idx < wsize; idx++) {
            mask[idx] &= 0xfffffff;
        }
        break;

    default:
        break;
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mem_range_write(int unit, bcmdrd_sid_t sid, int index_start,
                            int index_end, int tbl_inst, uint64_t buf)
{
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    int start, end, rv;

    SHR_FUNC_ENTER(unit);

    if (psim || asim) {
        /* The special test sequence does not apply on sim. */
        rv = bcma_testutil_cmicx_mem_range_write(unit, sid, index_start,
                                                 index_end, tbl_inst, buf);
        SHR_ERR_EXIT(rv);
    }

    /*
     * MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm is logical view of
     * MMU_THDO_BST_TOTAL_QUEUEm.
     * (MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm is the same case).
     * CPU write to MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm can be done by accessing
     * the MMU_THDO_BST_TOTAL_QUEUEm instead.
     * A DMA read to this memory will return four consecutive
     * MMU_THDO_BST_TOTAL_QUEUEm counts beginning with the memory
     * location that is on a boundary of 4.
     */

    switch (sid) {
    case MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm:
    case MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm:
        if (sid == MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm) {
            sid = MMU_THDO_BST_SHARED_PORTSP_MCm;
        } else {
            sid = MMU_THDO_BST_TOTAL_QUEUEm;
        }

        if (index_start <= index_end) {
            start = index_start * 4;
            end = index_end * 4 + 3;
        } else {
            start = index_end * 4;
            end = index_start * 4 + 3;
        }
        break;

    default:
        start = index_start;
        end = index_end;
        break;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_cmicx_mem_range_write(unit, sid, start, end,
                                             tbl_inst, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mem_range_read(int unit, bcmdrd_sid_t sid, int index_start,
                           int index_end, int tbl_inst, uint64_t buf)
{
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    int start, end, rv;

    SHR_FUNC_ENTER(unit);

    if (psim || asim) {
        /* The special test sequence does not apply on sim. */
        rv = bcma_testutil_cmicx_mem_range_read(unit, sid, index_start,
                                                index_end, tbl_inst, buf);
        SHR_ERR_EXIT(rv);
    }

    switch (sid) {
    case MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm:
    case MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm:
        if (index_start <= index_end) {
            start = index_start;
            end = index_end;
        } else {
            start = index_end;
            end = index_start;
        }
        break;

    default:
        start = index_start;
        end = index_end;
        break;
    }

    SHR_IF_ERR_EXIT
        (bcma_testutil_cmicx_mem_range_read(unit, sid, start, end,
                                            tbl_inst, buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pt_write(int unit,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info,
                     uint32_t *data)
{
    bool psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bool asim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    bcmbd_pt_dyn_info_t my_dyn_info;
    bcmdrd_sid_t int_set_sid;
    uint32_t my_data[2];
    int idx, wsize, offset, rv;

    SHR_FUNC_ENTER(unit);

    if (psim || asim) {
        /* The special test sequence does not apply on sim. */
        rv = bcmbd_pt_write(unit, sid, dyn_info, ovrr_info, data);
        SHR_ERR_EXIT(rv);
    }

    switch (sid) {
    case MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm:
    case MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm:
        if (sid == MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm) {
            sid = MMU_THDO_BST_SHARED_PORTSP_MCm;
        } else {
            sid = MMU_THDO_BST_TOTAL_QUEUEm;
        }

        my_dyn_info.tbl_inst = dyn_info->tbl_inst;
        idx = dyn_info->index * 4;
        for (offset = 0; offset < 4; offset++) {
            my_dyn_info.index = idx + offset;
            SHR_IF_ERR_EXIT
                (bcmbd_pt_write(unit, sid, &my_dyn_info, ovrr_info,
                                &data[offset]));
        }
        break;

    case MMU_EBPCC_CPU_INT_STATr:
    case MMU_THDR_QE_CPU_INT_STATr:
    case MMU_THDR_QE_CPU_INT_STAT_INSTr:
        if (sid == MMU_EBPCC_CPU_INT_STATr) {
            int_set_sid = MMU_EBPCC_CPU_INT_SETr;
        } else if (sid == MMU_THDR_QE_CPU_INT_STATr) {
            int_set_sid = MMU_THDR_QE_CPU_INT_SETr;
        } else {
            int_set_sid = MMU_THDR_QE_CPU_INT_SET_INSTr;
        }

        /*
         * Clear INT_STAT register:
         * step 1: write 0 into INT_SET register
         * step 2: write old value into INT_STAT register
         */
        sal_memset(my_data, 0, sizeof(my_data));
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, int_set_sid, dyn_info, ovrr_info, my_data));

        wsize = bcmdrd_pt_entry_wsize(unit, sid);
        SHR_IF_ERR_EXIT
            (bcmbd_pt_read(unit, sid, dyn_info, NULL, my_data, wsize));
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, sid, dyn_info, ovrr_info, my_data));

        /* Write new value into INT_SET register */
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, int_set_sid, dyn_info, ovrr_info, data));
        break;

    default:
        SHR_IF_ERR_EXIT
            (bcmbd_pt_write(unit, sid, dyn_info, ovrr_info, data));
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mem_cambist_info(int unit, bcmdrd_sid_t sid, bool *is_bist,
                             bcma_testutil_mem_cambist_entry_t *info)
{
#define CAMBIST_STATUS_EXPECTED (0x1f)

    const char *control_name = bcmdrd_pt_sid_to_name(unit, sid);
    char result_name[BCMA_TESTUTIL_STR_MAX_SIZE];
    const char *control_postfix = "CAM_BIST_CONFIG";
    const char *result_postfix[2]  = {"BIST_STATUSr", "CAM_BIST_STATUSr"};
    char *result_prefix;
    bcmdrd_fid_t *fields_list = NULL;
    size_t fields_num, i, prefix_len;
    uint32_t result_postfix_num = sizeof(result_postfix) /
                                  sizeof(result_postfix[0]);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(is_bist, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    *is_bist = false;

    /* check register name */
    if (control_name == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (sal_strstr(control_name, control_postfix) == NULL) {
        SHR_EXIT();
    }
    info->control_reg = sid;

    /* replace control reg if need */
    if (sal_strcmp(control_name, "IFP_TCAM_POOL1_CAM_BIST_CONFIG_64r") == 0 ||
        sal_strcmp(control_name, "IFP_TCAM_POOL2_CAM_BIST_CONFIG_64r") == 0) {
        /*
         * All IFP_TCAM_POOL[0/1/2] result share
         * the same single config of IFP_TCAM_POOL0
         */
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_name_to_sid(unit, "IFP_TCAM_POOL0_CAM_BIST_CONFIG_64r",
                                   &info->control_reg));
    }

    /* fetch enable/reset/mode field in this register */
    info->control_field_reset = INVALIDf;
    info->control_field_mode = INVALIDf;
    info->control_field_enable_num = 0;
    for (i = 0; i < BCMA_TESTUTIL_MEM_CAMBIST_FIELD_ENABLE_MAX; i++) {
        info->control_field_enable_arr[i] = INVALIDf;
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, info->control_reg, 0,
                                NULL, &fields_num));
    SHR_ALLOC(fields_list, fields_num * sizeof(bcmdrd_fid_t),
              "bcmaTestUtilFieldsList");
    SHR_NULL_CHECK(fields_list, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, info->control_reg, fields_num,
                                fields_list, &fields_num));

    for (i = 0; i < fields_num; i++) {
        bcmdrd_fid_t fid = fields_list[i];
        bcmdrd_sym_field_info_t finfo;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_info_get(unit, info->control_reg, fid, &finfo));

        if (finfo.name == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        /*
         * For TH4, there should be only one enable/reset/mode field
         * in the register
         */
        if (sal_strstr(finfo.name, "BIST_EN") != NULL) {
            if (info->control_field_enable_num != 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "there are multiple enable fields \
                                       in %s\n"),
                           control_name));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            info->control_field_enable_num = 1;
            info->control_field_enable_arr[0] = fid;

            if (sal_strstr(control_name, "EFP_CAM")) {
                info->control_field_enable_arr_count[0] = 8;
            } else if (sal_strstr(control_name, "IFP_TCAM_POOL")) {
                info->control_field_enable_arr_count[0] = 8;
            } else if (sal_strstr(control_name, "L3_DEFIP_CAM")) {
                info->control_field_enable_arr_count[0] = 14;
            } else if (sal_strstr(control_name, "VFP_CAM")) {
                info->control_field_enable_arr_count[0] = 16;
            } else {
                info->control_field_enable_arr_count[0] = 2;
            }
        }

        if (sal_strstr(finfo.name, "BIST_RST") != NULL) {
            if (info->control_field_reset != INVALIDf) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "there are multiple reset fields \
                                       in %s\n"),
                           control_name));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            info->control_field_reset = fid;
        }

        if (sal_strstr(finfo.name, "BIST_MODE") != NULL) {
            if (info->control_field_mode != INVALIDf) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "there are multiple mode fields \
                                       in %s\n"),
                           control_name));
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            info->control_field_mode = fid;
            info->control_field_mode_num = 1;
            info->control_field_mode_arr_value[0] = 0; /* single mode only */
        }
    }
    if (info->control_field_enable_num == 0) {
        SHR_EXIT(); /* enable field is must have */
    }

    /*
     * find the corresponding status register name
     *   ex.    EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_BIST_CONFIG_64r
     *      ==> EXACT_MATCH_LOGICAL_TABLE_SELECT_CAM_BIST_STATUSr
     */
    info->result_reg = INVALIDr;
    for (i = 0; i < result_postfix_num; i++) {
        if (sal_strlen(control_name) + 1 > BCMA_TESTUTIL_STR_MAX_SIZE) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_memcpy(result_name, control_name, sal_strlen(control_name) + 1);
        result_prefix = sal_strstr(result_name, control_postfix);
        if (result_prefix == NULL) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        prefix_len = sal_strlen(result_name) - sal_strlen(result_prefix);
        if (prefix_len + sal_strlen(result_postfix[i]) + 1
            > BCMA_TESTUTIL_STR_MAX_SIZE) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_memcpy(result_prefix, result_postfix[i],
                   sal_strlen(result_postfix[i]) + 1);

        if (SHR_SUCCESS
                (bcmdrd_pt_name_to_sid(unit, result_name, &info->result_reg))) {
             break;
        }
    }
    if (info->result_reg == INVALIDr) {
        LOG_WARN(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "cannot find status reg from %s\
                                   (sid = %"PRId32")\n"),
                                  control_name, info->control_reg));
             SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* fetch field in the result register */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, info->result_reg, 0, NULL, &fields_num));
    if (fields_num != 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "there are multiple fields in %s\n"),
                   result_name));
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, info->result_reg, fields_num,
                                fields_list, &fields_num));
    info->result_field_status = fields_list[0];
    info->result_field_status_expect = CAMBIST_STATUS_EXPECTED;
    *is_bist = true;

exit:
    SHR_FREE(fields_list);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_pt_entity_override(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info,
                               bcma_testutil_pt_entity_handle_f entity_handle_cb,
                               void *user_data)
{
    int blktype, acctype, rv;
    uint32_t ibmp = 0;
    int pipe_num = bcmdrd_pt_num_pipe_inst(unit, sid);
    int pipe_idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(entity_handle_cb, SHR_E_PARAM);

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    if (SHR_FAILURE(bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &ibmp))) {
        SHR_EXIT();
    }

    acctype = bcmdrd_pt_acctype_get(unit, sid);
    switch (acctype) {
        case AT_DUPLICATE:
            break;
        case AT_DUPLICATE2:
            /* DUPLICATE2 is used to access pipe 0 and 2 */
            ibmp = ibmp & 0x5;
            break;
        default:
            SHR_EXIT();
    }

    /* for duplicate type, iterate all available pipes with unique access */
    for (pipe_idx = 0; pipe_idx < pipe_num; pipe_idx++) {
        if ((1 << pipe_idx) & ibmp) {
            bcmbd_cmicx_ovrr_info_t ovrr;
            ovrr.inst = pipe_idx;
            rv = entity_handle_cb(unit, sid, dyn_info, &ovrr, user_data);
            if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static const uint32_t *
bcm56990_a0_pt_default_value_get(int unit, bcmdrd_sid_t sid)
{
    /*
     * The default value for these tables has been modified by SW patch,
     * restore it back to HW value.
     */
    if (sid == LPORT_TABm || sid == L3_MTU_VALUESm) {
        return (const uint32_t *)default_all_zero;
    }

    return bcmdrd_pt_default_value_get(unit, sid);
}

bcma_testutil_drv_power_op_t bcm56990_a0_power_op = {
    .power_help         = bcm56990_a0_power_help,
    .power_init         = bcm56990_a0_power_init,
    .power_port_set     = bcm56990_a0_power_port_set,
    .power_config_set   = bcm56990_a0_power_config_set,
    .power_txrx         = bcm56990_a0_power_txrx
};

int
bcm56990_a0_testutil_drv_attach(int unit)
{
    bcma_testutil_drv_t *testutil_drv;

    testutil_drv = bcma_testutil_drv_get(unit);
    if (testutil_drv == NULL) {
        return SHR_E_FAIL;
    }

    testutil_drv->pt_skip = bcm56990_a0_pt_skip;
    testutil_drv->pt_hw_update = bcm56990_a0_pt_hw_update;
    testutil_drv->mem_address_adjust = bcm56990_a0_mem_address_adjust;
    testutil_drv->mem_hash_data_adjust = bcm56990_a0_mem_hash_data_adjust;
    testutil_drv->mem_range_write = bcm56990_a0_mem_range_write;
    testutil_drv->mem_range_read = bcm56990_a0_mem_range_read;
    testutil_drv->mem_mbist_info = bcm56990_a0_mem_mbist_info;
    testutil_drv->mem_mbist_dma_info = bcm56990_a0_mem_mbist_dma_info;
    testutil_drv->cmc_chan_get = bcm56990_a0_cmc_chan_get;
    testutil_drv->benchmark_reglist_get = bcm56990_a0_benchmark_reglist_get;
    testutil_drv->benchmark_memlist_get = bcm56990_a0_benchmark_memlist_get;
    testutil_drv->mem_cambist_info = bcm56990_a0_mem_cambist_info;
    testutil_drv->vlan_op_get = bcma_testutil_xgs_vlan_op_get;
    testutil_drv->l2_op_get = bcma_testutil_xgs_l2_op_get;
    testutil_drv->ctr_op_get = bcma_testutil_port_ctr_op_get;
    testutil_drv->flex_traffic_op_get = xgs_flex_traffic_op_get;
    testutil_drv->flex_lt_op_get = bcma_testutil_xfs_flex_lt_op_get;
    testutil_drv->traffic_default_pktcnt = 1200;
    testutil_drv->traffic_expeceted_rate_get = bcm56990_a0_traffic_get_expeceted_rate;
    testutil_drv->traffic_fixed_packet_list_get = bcm56990_a0_traffic_get_fixed_packet_list;
    testutil_drv->traffic_random_packet_list_get = bcm56990_a0_traffic_get_random_packet_list;
    testutil_drv->traffic_worstcase_pktlen_get = bcm56990_a0_traffic_get_worstcase_pktlen;
    testutil_drv->power_op_get = &bcm56990_a0_power_op;
    testutil_drv->chip_blktype_get = bcm56990_a0_chip_blktype_get;
    testutil_drv->pkt_cfg_data_get = bcm56990_a0_pkt_cfg_data_get;
    testutil_drv->pt_mask_override = bcm56990_a0_pt_mask_override;
    testutil_drv->pt_entity_override = bcm56990_a0_pt_entity_override;
    testutil_drv->pt_write = bcm56990_a0_pt_write;
    testutil_drv->pt_read = bcmbd_pt_read;
    testutil_drv->qos_lt_op_get = bcma_testutil_xgs_qos_lt_op_get;
    testutil_drv->check_health = bcm56990_a0_check_health;
    testutil_drv->pt_default_value_get = bcm56990_a0_pt_default_value_get;

    SHR_BITSET(testutil_drv->features,
               BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD);
    SHR_BITSET(testutil_drv->features,
               BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID);

    return SHR_E_NONE;
}
