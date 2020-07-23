/*! \file bcm56780_a0_testutil_drv.c
 *
 * Chip-specific driver utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_assert.h>

#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmbd/chip/bcm56780_a0_dev.h>
#include <bcmbd/bcmbd_cmicx.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>
#include <bcma/test/util/bcma_testutil_cmicx.h>
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/chip/bcm56780_a0_testutil_traffic.h>
#include <bcmltd/bcmltd_lt_types.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define CMC_NUM 2
#define CMC_CHAN_NUM_PER_CMC 8


static bcmdrd_sid_t bcm56780_a0_reg_reset_skip_pt[] = {
    /* These tables are modified in device init phase. */
    MMU_GLBCFG_MISCCONFIGr,
    DLB_ECMP_HW_RESET_CONTROLr,
    CDPORT_SBUS_CONTROLr,

    /* Hardware interrupt and status registers. */
    MMU_EBPCC_CPU_INT_STATr,
    MMU_RQE_INFOTBL_FP_INITr,
    ING_DII_HW_STATUSr,
    EGR_DII_HW_STATUSr,
    EGR_DOI_EVENT_FIFO_STATUSr,
    ETRAP_INTR_STATUSr,
    IDB_CA4_HW_STATUS_1r,
    IDB_OBM4_CTRL_ECC_STATUSr,
    IDB_OBM4_DATA_ECC_STATUSr,
    ING_DOI_EVENT_FIFO_STATUSr,
    PORT_IF_SBS_CONTROLr,
    AVS_PMB_SLAVE_AVS_ROSC_LVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_LVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT8_THRESHOLDr,
    AVS_TMON_SPARE_0r,
    MGMT_OBM_BUFFER_CONFIGr,
    TOP_MACSEC_CTRL0r,
};

static bcmdrd_sid_t bcm56780_a0_reg_wr_skip_pt[] = {
    AVS_PMB_SLAVE_AVS_ROSC_LVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_LVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_SVT8_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT11_THRESHOLDr,
    AVS_PMB_SLAVE_AVS_ROSC_ULVT8_THRESHOLDr,
    AVS_TMON_SPARE_0r,

    /* Hardware interrupt and status registers. */
    MMU_EBPCC_CPU_INT_STATr,
    MMU_RQE_INFOTBL_FP_INITr,
    ING_DII_HW_STATUSr,
    EGR_DII_HW_STATUSr,
    MMU_THDR_QE_CPU_INT_STATr,
    MMU_RQE_QUEUE_SNAPSHOT_ENr,
    MMU_INTFO_HW_UPDATE_DISr,
    ING_DOI_EVENT_FIFO_STATUSr,
    IDB_OBM4_DATA_ECC_STATUSr,
    IDB_CA4_HW_STATUS_1r,
    ETRAP_INTR_STATUSr,
    EGR_DOI_EVENT_FIFO_STATUSr,
    IDB_OBM4_OVERSUB_MON_ECC_STATUSr,

    /* hw_write. */
    IDB_OBM_MONITOR_CONFIGr,

    PORT_IF_SBS_CONTROLr,
    TOP_MACSEC_CTRL0r,
    DLB_ECMP_HW_RESET_CONTROLr,

    /* The sequence has to be write -> wait timeout -> read. */
    PBLOCK_MISC_EP0_BYPASS_CONTROLr,
    PBLOCK_MISC_EP1_1_BYPASS_CONTROLr,
    PBLOCK_MISC_EP2_0_BYPASS_CONTROLr,
    PBLOCK_MISC_IP0_BYPASS_CONTROLr,
    PBLOCK_MISC_IP1_BYPASS_CONTROLr,
    PBLOCK_MISC_IP3_0_BYPASS_CONTROLr,
    PBLOCK_MISC_IP3_1_BYPASS_CONTROLr,
    PBLOCK_MISC_IP7_BYPASS_CONTROLr,
    PBLOCK_MISC_IP8_0_BYPASS_CONTROLr,
    PBLOCK_MISC_IP9_0_BYPASS_CONTROLr,
    PBLOCK_MISC_IP9_1_BYPASS_CONTROLr,
    MGMT_OBM_BUFFER_CONFIGr,
    MGMT_OBM_MONITOR_CONFIGr, /* CONFIG_ERRORf is hw_write. */
};

static bcmdrd_sid_t bcm56780_a0_mem_skip_pt[] = {
    /*
     * These tables are used to configure UFT table attributes,
     * such as bucket mode, key width.
     */
    IFTA40_E2T_00_KEY_ATTRIBUTESm,
    IFTA90_E2T_01_KEY_ATTRIBUTESm,
    IFTA80_E2T_02_KEY_ATTRIBUTESm,
    IFTA80_E2T_03_KEY_ATTRIBUTESm,
    EFTA30_E2T_00_KEY_ATTRIBUTESm,
    IFTA30_E2T_00_KEY_ATTRIBUTESm,
    IFTA80_E2T_01_KEY_ATTRIBUTESm,
    IFTA90_E2T_00_KEY_ATTRIBUTESm,
    IFTA40_E2T_01_KEY_ATTRIBUTESm,
    IFTA80_E2T_00_KEY_ATTRIBUTESm,
    M_E2T_00_KEY_ATTRIBUTESm,
    M_E2T_01_KEY_ATTRIBUTESm,

    /* DMA read only. */
    MMU_THDO_BST_SHARED_PORTSP_MC_PKTSTATm,
    MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm,
    M_TOP_MEM0_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM1_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM2_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM3_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM4_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM5_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM6_TCAM_SRAM_DEBUG_DATAm,
    M_TOP_MEM7_TCAM_SRAM_DEBUG_DATAm,

    /* CPU_W => 0, CPU_R => 0. */
    MMU_EBMB_CCBE_SLICEm,
    MMU_EBMB_PAYLOAD_SLICEm,

    /* NON_SBUS_ACC  => 1. */
    MMU_EBMB_PAYLOAD_ITM0_CH0H_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH0L_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH1H_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH1L_SERm,

    /* Write only. */
    FT_COMMANDm,
    MEMDB_TCAM_IFTA80_MEM0_0m,
    MEMDB_TCAM_IFTA80_MEM0_1m,
    MEMDB_TCAM_IFTA80_MEM1_0m,
    MEMDB_TCAM_IFTA80_MEM1_1m,
    MEMDB_TCAM_IFTA80_MEM2_0m,
    MEMDB_TCAM_IFTA80_MEM2_1m,
    MEMDB_TCAM_IFTA80_MEM3_0m,
    MEMDB_TCAM_IFTA80_MEM3_1m,
    MEMDB_TCAM_IFTA80_MEM4_0m,
    MEMDB_TCAM_IFTA80_MEM4_1m,
    MEMDB_TCAM_IFTA80_MEM5_0m,
    MEMDB_TCAM_IFTA80_MEM5_1m,
    MEMDB_TCAM_IFTA80_MEM6_0m,
    MEMDB_TCAM_IFTA80_MEM6_1m,
    MEMDB_TCAM_IFTA80_MEM7_0m,
    MEMDB_TCAM_IFTA80_MEM7_1m,
    MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m,
    MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m,
    MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m,
    MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m,
    MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m,
    MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m,
    MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m,
    MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m,
    MEMDB_TCAM_M_CTL_MEM0_0m,
    MEMDB_TCAM_M_CTL_MEM0_1m,
    MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m,
    MEMDB_TCAM_M_CTL_MEM1_0m,
    MEMDB_TCAM_M_CTL_MEM1_1m,
    MEMDB_TCAM_M_CTL_MEM1_0_MEM1_1m,
    MEMDB_TCAM_M_CTL_MEM2_0m,
    MEMDB_TCAM_M_CTL_MEM2_1m,
    MEMDB_TCAM_M_CTL_MEM2_0_MEM2_1m,
    MEMDB_TCAM_M_CTL_MEM3_0m,
    MEMDB_TCAM_M_CTL_MEM3_0_MEM3_1m,
    MEMDB_TCAM_M_CTL_MEM3_1m,
    MEMDB_TCAM_M_CTL_MEM4_0m,
    MEMDB_TCAM_M_CTL_MEM4_0_MEM4_1m,
    MEMDB_TCAM_M_CTL_MEM4_1m,
    MEMDB_TCAM_M_CTL_MEM5_0m,
    MEMDB_TCAM_M_CTL_MEM5_0_MEM5_1m,
    MEMDB_TCAM_M_CTL_MEM5_1m,
    MEMDB_TCAM_M_CTL_MEM6_0m,
    MEMDB_TCAM_M_CTL_MEM6_0_MEM6_1m,
    MEMDB_TCAM_M_CTL_MEM6_1m,
    MEMDB_TCAM_M_CTL_MEM7_0m,
    MEMDB_TCAM_M_CTL_MEM7_0_MEM7_1m,
    MEMDB_TCAM_M_CTL_MEM7_1m,

    /* ETRAP interrupt. */
    ETRAP_SAMPLE_FLOW_COUNT_LEFTm,
    ETRAP_SAMPLE_FLOW_COUNT_RIGHTm,
    ETRAP_SAMPLE_FLOW_CTRL_LEFTm,
    ETRAP_SAMPLE_FLOW_CTRL_RIGHTm,
    ETRAP_SAMPLE_FLOW_HASH_L0m,
    ETRAP_SAMPLE_FLOW_HASH_L1m,
    ETRAP_SAMPLE_FLOW_HASH_L2m,
    ETRAP_SAMPLE_FLOW_HASH_L3m,
    ETRAP_SAMPLE_FLOW_HASH_L4m,
    ETRAP_SAMPLE_FLOW_HASH_R0m,
    ETRAP_SAMPLE_FLOW_HASH_R1m,
    ETRAP_SAMPLE_FLOW_HASH_R2m,
    ETRAP_SAMPLE_FLOW_HASH_R3m,
    ETRAP_SAMPLE_FLOW_HASH_R4m,

    /* Interrupt and status memories. */
    IPOST_SER_STATUS_BLK_ING_SER_FIFOm,
    EPOST_EGR_SER_FIFOm,
    EGR_METADATA_EXTRACTION_FIFO_SINGLEm,
    EGR_METADATA_EXTRACTION_FIFO_QUADm,
    EGR_METADATA_EXTRACTION_FIFO_DOUBLEm,
    EGR_COMPOSITES_EGR_COMPOSITES_BUFFERm,
    DLB_ECMP_SER_FIFOm,

    /* PRG_CATEGORY => RAMCTRL. */
    IFTA30_E2T_00_RAM_CONTROLm,
    EFTA30_E2T_00_RAM_CONTROLm,
    IFTA40_E2T_00_RAM_CONTROLm,
    IFTA40_E2T_01_RAM_CONTROLm,
    IFTA80_E2T_00_RAM_CONTROLm,
    IFTA80_E2T_01_RAM_CONTROLm,
    IFTA80_E2T_03_RAM_CONTROLm,
    IFTA80_E2T_02_RAM_CONTROLm,
    IFTA90_E2T_00_RAM_CONTROLm,
    IFTA90_E2T_01_RAM_CONTROLm,
    L3_DEFIP_ALPM_LEVEL2_RAM_CONTROLm,
    M_E2T_00_RAM_CONTROLm,
    M_E2T_01_RAM_CONTROLm,
    M_ALPM_LEVEL2_RAM_CONTROLm,
    M_ALPM_LEVEL3_RAM_CONTROLm,

    /* HW_UPDATE => 1 */
    EFP_METER_METER_TABLEm,
    /* By design, register not accessible when lane mode is set to 5. */
    M_LPM_IP_CONTROLm,
};

static const bcmdrd_sid_t bcm56780_a0_mem_ecc[] = {
    IFTA30_E2T_00_B1_ECCm,
    IFTA80_E2T_01_B0_ECCm,
    IFTA40_E2T_01_B1_ECCm,
    IFTA90_E2T_01_B0_ECCm,
    IFTA90_E2T_01_B2_ECCm,
    IFTA90_E2T_01_B3_ECCm,
    IFTA40_E2T_00_B0_ECCm,
    EFTA30_E2T_00_B0_ECCm,
    EFTA30_E2T_00_B1_ECCm,
    IFTA80_E2T_00_B1_ECCm,
    IFTA40_E2T_01_B0_ECCm,
    IFTA80_E2T_00_B0_ECCm,
    IFTA30_E2T_00_B0_ECCm,
    IFTA90_E2T_00_B0_ECCm,
    IFTA80_E2T_02_B0_ECCm,
    IFTA80_E2T_03_B0_ECCm,
    IFTA90_E2T_01_B1_ECCm,
    IFTA40_E2T_00_B1_ECCm,
    IFTA80_E2T_02_B1_ECCm,
    IFTA90_E2T_00_B1_ECCm,
    IFTA90_E2T_00_B2_ECCm,
    IFTA90_E2T_00_B3_ECCm,
    IFTA80_E2T_01_B1_ECCm,
    IFTA80_E2T_03_B1_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B0_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B5_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B5_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B2_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B4_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B0_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B1_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B3_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B1_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B2_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B7_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B6_ECCm,
    L3_DEFIP_ALPM_LEVEL3_B4_ECCm,
    L3_DEFIP_ALPM_LEVEL2_B3_ECCm,
    M_DEFIP_ALPM_LEVEL3_B0_ECCm,
    M_DEFIP_ALPM_LEVEL3_B0_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B1_ECCm,
    M_DEFIP_ALPM_LEVEL3_B1_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B2_ECCm,
    M_DEFIP_ALPM_LEVEL3_B2_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B3_ECCm,
    M_DEFIP_ALPM_LEVEL3_B3_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B4_ECCm,
    M_DEFIP_ALPM_LEVEL3_B4_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B5_ECCm,
    M_DEFIP_ALPM_LEVEL3_B5_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B6_ECCm,
    M_DEFIP_ALPM_LEVEL3_B6_ECC_2m,
    M_DEFIP_ALPM_LEVEL3_B7_ECCm,
    M_DEFIP_ALPM_LEVEL3_B7_ECC_2m,
};

static shr_enum_map_t bcm56780_a0_blktype_name_id_map[] = {
    {"avs", BLKTYPE_AVS},
    {"cdport", BLKTYPE_CDPORT},
    {"cev", BLKTYPE_CEV},
    {"cmic", BLKTYPE_CMIC},
    {"epipe", BLKTYPE_EPIPE},
    {"ipipe", BLKTYPE_IPIPE},
    {"iproc", BLKTYPE_IPROC},
    {"lbport", BLKTYPE_LBPORT},
    {"mmu_eb", BLKTYPE_MMU_EB},
    {"mmu_glb", BLKTYPE_MMU_GLB},
    {"mmu_itm", BLKTYPE_MMU_ITM},
    {"port_if", BLKTYPE_PORT_IF},
    {"ser", BLKTYPE_SER},
    {"top", BLKTYPE_TOP},
    {"xlport", BLKTYPE_XLPORT}
};

static bcmdrd_sid_t bcm56780_a0_tr144_skip_pt_list[] = {
    /* Can't trigger SER error */
    IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM3_IOM_STATS_WINDOW_RESULTSm,
    IDB_OBM4_IOM_STATS_WINDOW_RESULTSm,
    MGMT_OBM_IOM_STATS_WINDOW_RESULTSm,
    /* Can't access internal PTs */
    MMU_EBMB_PAYLOAD_ITM0_CH0H_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH0L_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH1H_SERm,
    MMU_EBMB_PAYLOAD_ITM0_CH1L_SERm,
    MMU_EBMB_PAYLOAD_ITM1_CH0H_SERm,
    MMU_EBMB_PAYLOAD_ITM1_CH0L_SERm,
    MMU_EBMB_PAYLOAD_ITM1_CH1H_SERm,
    MMU_EBMB_PAYLOAD_ITM1_CH1L_SERm,
    RDB0_BANK_0_0m,
    RDB0_BANK_0_1m,
    RDB0_BANK_0_2m,
    RDB0_BANK_1_0m,
    RDB0_BANK_1_1m,
    RDB0_BANK_1_2m,
    RDB0_BANK_2_0m,
    RDB0_BANK_2_1m,
    RDB0_BANK_2_2m,
    RDB0_BANK_3_0m,
    RDB0_BANK_3_1m,
    RDB0_BANK_3_2m,
    RDB0_CELm,
    RDB0_CXT_ABm,
    RDB0_CXT_CDm,
    RDB0_FCP_0m,
    RDB0_FCP_1m,
    RDB0_FCP_2m,
    RDB0_FCP_3m,
    RDB0_PKTm,
    RDB1_BANK_0_0m,
    RDB1_BANK_0_1m,
    RDB1_BANK_0_2m,
    RDB1_BANK_1_0m,
    RDB1_BANK_1_1m,
    RDB1_BANK_1_2m,
    RDB1_BANK_2_0m,
    RDB1_BANK_2_1m,
    RDB1_BANK_2_2m,
    RDB1_BANK_3_0m,
    RDB1_BANK_3_1m,
    RDB1_BANK_3_2m,
    RDB1_CELm,
    RDB1_CXT_ABm,
    RDB1_CXT_CDm,
    RDB1_FCP_0m,
    RDB1_FCP_1m,
    RDB1_FCP_2m,
    RDB1_FCP_3m,
    RDB1_PKTm,

};

static const bcma_testutil_pkt_lt_cfg_t bcm56780_a0_pkt_traffic_lt_cfg[] = {
{"ING_DVP_TABLE",
     {{"DVP", 0}, {"STRENGTH_PROFILE_INDEX", 1}}, BCMLT_OPCODE_INSERT},
    {"ING_DVP_STRENGTH_PROFILE",
     {{"ING_DVP_STRENGTH_PROFILE_INDEX", 1}, {"L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0}, {"DESTINATION_STRENGTH", 0}},
     BCMLT_OPCODE_INSERT},
    {"ING_L3_NEXT_HOP_1_TABLE",
     {{"NHOP_INDEX_1", 0}, {"STRENGTH_PROFILE_INDEX", 1}}, BCMLT_OPCODE_INSERT},
    {"ING_L3_NEXT_HOP_1_STRENGTH_PROFILE",
     {{"ING_L3_NEXT_HOP_1_STRENGTH_PROFILE_INDEX", 1}, {"L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0}, {"DESTINATION_STRENGTH", 0}},
     BCMLT_OPCODE_INSERT},
    {"ING_L3_NEXT_HOP_2_TABLE",
     {{"NHOP_INDEX_2", 0}, {"STRENGTH_PROFILE_INDEX", 2}}, BCMLT_OPCODE_INSERT},
    {"ING_L3_NEXT_HOP_2_STRENGTH_PROFILE",
     {{"ING_L3_NEXT_HOP_2_STRENGTH_PROFILE_INDEX", 2}, {"L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTH", 0}, {"DESTINATION_STRENGTH", 0}},
     BCMLT_OPCODE_INSERT},
    {"PORT", {{"PORT_ID", 41}, {"ING_SYSTEM_PORT_TABLE_ID", 41}}, BCMLT_OPCODE_INSERT},
    {"PORT", {{"PORT_ID", 2}, {"ING_SYSTEM_PORT_TABLE_ID", 2}}, BCMLT_OPCODE_INSERT},
    {"PORT_PROPERTY",
     {{"PORT_ID", 41}, {"PORT_PARSER", 3}, {"PORT_TYPE", 1}}, BCMLT_OPCODE_INSERT},
    {"PORT_PROPERTY",
     {{"PORT_ID", 2}, {"PORT_PARSER", 3}, {"PORT_TYPE", 1}}, BCMLT_OPCODE_INSERT},
    {"ING_SYSTEM_PORT_TABLE",
     {{"SYSTEM_SOURCE", 41}, {"OUTER_L2HDR_OTPID_ENABLE", 0xf}, {"L2_IIF", 0x2},
      {"L2_IIF_STRENGTH", 1}, {"INGRESS_PP_PORT", 41}},
     BCMLT_OPCODE_INSERT},
    {"ING_SYSTEM_PORT_TABLE",
     {{"SYSTEM_SOURCE", 2}, {"OUTER_L2HDR_OTPID_ENABLE", 0xf}, {"L2_IIF", 0x2},
      {"L2_IIF_STRENGTH", 1}, {"INGRESS_PP_PORT", 2}},
     BCMLT_OPCODE_INSERT},
    {"ING_L2_IIF_TABLE", {{"L2_IIF", 2}, {"VLAN_TO_VFI_MAPPING_ENABLE", 1}, {"VFI", 2}}, BCMLT_OPCODE_INSERT},
    {"VFI_STRENGTH_PROFILE",
     {{"VFI_STRENGTH_PROFILE_INDEX", 1}, {"L3_IIF_STRENGTH", 7}}, BCMLT_OPCODE_INSERT},
    {"ING_VFI_TABLE",
     {{"STRENGTH_PROFILE_INDEX", 1}, {"VFI", 2}, {"MAPPED_VFI", 2}, {"SPANNING_TREE_GROUP", 1}, {"MEMBERSHIP_PROFILE_PTR", 1}},
     BCMLT_OPCODE_INSERT},
    {"VLAN_ING_MEMBER_PROFILE",
     {{"VLAN_ING_MEMBER_PROFILE_ID", 1}, {"MEMBER_PORTS", 1, NULL, 42, 0}},
     BCMLT_OPCODE_INSERT},
    {"VLAN_ING_STG_PROFILE",
     {{"VLAN_ING_STG_PROFILE_ID", 1}, {"STATE", 0, "FORWARD", 42, 0}}, BCMLT_OPCODE_INSERT},
    {"VLAN_ING_STG_PROFILE",
     {{"VLAN_ING_STG_PROFILE_ID", 1}, {"STATE", 0, "LEARN", 42, 41}}, BCMLT_OPCODE_UPDATE},
    {"VLAN_ING_EGR_MEMBER_PORTS_PROFILE",
     {{"VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID", 1}, {"EGR_MEMBER_PORTS", 1, NULL, 42, 0}},
     BCMLT_OPCODE_INSERT},
    {"VLAN_ING_EGR_STG_MEMBER_PROFILE",
     {{"VLAN_ING_EGR_STG_MEMBER_PROFILE_ID", 1}, {"STATE", 0, "FORWARD", 42, 0}, {"COMPARED_STATE", 0, "FORWARD", 1, 0}},
     BCMLT_OPCODE_INSERT},
    {"DEVICE_EM_TILE",
     {{"DEVICE_EM_TILE_ID", 0, "IFTA80_E2T_02"}, {"MODE", 0, "IFTA80_E2T_02_MODE0"}}, BCMLT_OPCODE_UPDATE},
    {"DEVICE_EM_GROUP",
     {{"DEVICE_EM_GROUP_ID", 0, "IFTA80_E2T_02_MODE0_GROUP0"}, {"NUM_BANKS", 2}, {"DEVICE_EM_BANK_ID", 0, "IFTA80_E2T_02_BANK0", 1, 0},
      {"DEVICE_EM_BANK_ID", 0, "IFTA80_E2T_02_BANK1", 2, 1}},
 BCMLT_OPCODE_UPDATE},
    {"TABLE_CONTROL",
     {{"TABLE_ID", 0, "L2_HOST_TABLE"}, {"MAX_ENTRIES", 8192}}, BCMLT_OPCODE_UPDATE},
    {"L2_HOST_TABLE",
     {{"MAC_ADDR", 0xBBBBBB}, {"VFI", 0x2}, {"STRENGTH_PROFILE_INDEX", 7}, {"DESTINATION", 41},
      {"DESTINATION_TYPE", 0, "L2_OIF"}, {"MISC_CTRL_0", 1}, {"MISC_CTRL_1", 8}},
     BCMLT_OPCODE_INSERT},
    {"L2_HOST_STRENGTH_PROFILE",
     {{"L2_HOST_STRENGTH_PROFILE_INDEX", 7}, {"L2_DST_DESTINATION_STRENGTH", 7}, {"L2_DST_DESTINATION_TYPE_STRENGTH", 7},
      {"L2_DST_MISC_CTRL_0_STRENGTH", 7}, {"L2_DST_MISC_CTRL_1_STRENGTH", 1}},
     BCMLT_OPCODE_INSERT},
    {"ING_SYSTEM_DESTINATION_TABLE",
     {{"SYSTEM_DESTINATION_PORT", 41}, {"L2_OIF", 41}, {"L2_OIF_STRENGTH", 1}}, BCMLT_OPCODE_INSERT},
    {"ING_SYSTEM_DESTINATION_TABLE",
     {{"SYSTEM_DESTINATION_PORT", 2}, {"L2_OIF", 2}, {"L2_OIF_STRENGTH", 1}}, BCMLT_OPCODE_INSERT},
    {"L2_EIF_SYSTEM_DESTINATION", {{"L2_EIF_ID", 41}, {"IS_TRUNK", 0}, {"SYSTEM_PORT", 41}}, BCMLT_OPCODE_INSERT},
    {"L2_EIF_SYSTEM_DESTINATION", {{"L2_EIF_ID", 2}, {"IS_TRUNK", 0}, {"SYSTEM_PORT", 2}}, BCMLT_OPCODE_INSERT},
    {"PORT_SYSTEM_DESTINATION",
     {{"PORT_SYSTEM_ID", 41}, {"IS_TRUNK_SYSTEM", 0}, {"PORT_ID", 41}}, BCMLT_OPCODE_INSERT},
    {"PORT_SYSTEM_DESTINATION",
     {{"PORT_SYSTEM_ID", 2}, {"IS_TRUNK_SYSTEM", 0}, {"PORT_ID", 2}}, BCMLT_OPCODE_INSERT},
    {"EGR_VFI",
     {{"VFI", 2}, {"VIEW_T", 0, "VT_VFI_DEFAULT"}, {"VLAN_0", 2}, {"TAG_ACTION", 0}, {"STG", 2},
      {"MEMBERSHIP_PROFILE_IDX", 2}},
     BCMLT_OPCODE_INSERT},
    {"VLAN_EGR_MEMBER_PROFILE",
     {{"VLAN_EGR_MEMBER_PROFILE_ID", 2}, {"MEMBER_PORTS", 1, NULL, 42, 0}}, BCMLT_OPCODE_INSERT},
    {"VLAN_EGR_STG_PROFILE",
     {{"VLAN_EGR_STG_PROFILE_ID", 2}, {"STATE", 0, "FORWARD", 42, 0}}, BCMLT_OPCODE_INSERT},
    {"EGR_L2_OIF",
     {{"L2_OIF", 41}, {"EDIT_CTRL_ID", 2}, {"TPID_0", 0x8100}, {"TPID_1", 0x9100}}, BCMLT_OPCODE_INSERT},
    {"EGR_L2_OIF",
     {{"L2_OIF", 2}, {"EDIT_CTRL_ID", 2}, {"TPID_0", 0x8100}, {"TPID_1", 0x9100}}, BCMLT_OPCODE_INSERT},
    {"PC_PORT", {{"PORT_ID", 41}, {"LOOPBACK_MODE", 0, "PC_LPBK_MAC"}}, BCMLT_OPCODE_UPDATE},
    {"PC_PORT", {{"PORT_ID", 2}, {"LOOPBACK_MODE", 0, "PC_LPBK_MAC"}}, BCMLT_OPCODE_UPDATE},
    {"LM_PORT_CONTROL", {{"PORT_ID", 41}}, BCMLT_OPCODE_INSERT},
    {"LM_PORT_CONTROL", {{"PORT_ID", 2}}, BCMLT_OPCODE_INSERT},
    {"LM_LINK_STATE", {{"PORT_ID", 41}}, BCMLT_OPCODE_LOOKUP},
    {"LM_LINK_STATE", {{"PORT_ID", 2}}, BCMLT_OPCODE_LOOKUP},
};
static uint8_t bcm56780_a0_pkt_raw_data[] = {
    00,00,00,0xBB,0xBB,0xBB,00,0x2A,0x10,0x77,0x77,00,0x08,00,
    0x45,00,00,0x3C,0x67,0x62,00,00,0xFF,0x01,0xBD,0x76,0x0A,0x3A,
    0x40,0x01,0x0A,0x3A,0x42,0x73,00,00,0x55,0x0A,00,0x01,00,0x51,
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x61,0x62,0x63,
    0x64,0x65,0x66,0x67,0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
    0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
    0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x61,0x62,
    0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,
    0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,
    0x73,0x74,0x75,0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,
    0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,
    0x76,0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,
    0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x61
};

static uint32_t bcm56780_a0_tx_pmd[][3] = {
    {0x81000000, 0x2, 0x6000},
};

static const bcma_testutil_pkt_data_t bcm56780_a0_pkt_data = {
    bcm56780_a0_tx_pmd,
    sizeof(bcm56780_a0_tx_pmd),
    NULL, 0, NULL, 0,
    bcm56780_a0_pkt_raw_data,
    sizeof(bcm56780_a0_pkt_raw_data)
};

bcmdrd_ser_rmr_id_t bcm56780_a0_bb_skip_pt[12] = {0};

static bool
bcm56780_a0_pt_skip(int unit, bcmdrd_sid_t sid, bcma_testutil_pt_type_t type)
{
    const bcmdrd_sid_t *skip_pt;
    int blktype, count = 0, idx;
    uint32_t inst_map = 0;
    const char *pt_name = NULL;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    switch (type) {
    case BCMA_TESTUTIL_PT_REG_RESET:
        skip_pt = bcm56780_a0_reg_reset_skip_pt;
        count = COUNTOF(bcm56780_a0_reg_reset_skip_pt);
        break;

    case BCMA_TESTUTIL_PT_REG_RD_WR:
        skip_pt = bcm56780_a0_reg_wr_skip_pt;
        count = COUNTOF(bcm56780_a0_reg_wr_skip_pt);
        break;

    case BCMA_TESTUTIL_PT_MEM_RD_WR:
    case BCMA_TESTUTIL_PT_MEM_DMA_RD_WR:
        /* Memory test only on EP, IP, MMU, SER */
        switch (blktype) {
        case BLKTYPE_EPIPE:
        case BLKTYPE_IPIPE:
        case BLKTYPE_MMU_GLB:
        case BLKTYPE_MMU_ITM:
        case BLKTYPE_MMU_EB:
        case BLKTYPE_SER:
        /* Block for EDB, IDB */
        case BLKTYPE_PORT_IF:
            skip_pt = bcm56780_a0_mem_skip_pt;
            count = COUNTOF(bcm56780_a0_mem_skip_pt);
            break;
        default:
            return TRUE;
        }
        break;

    case BCMA_TESTUTIL_PT_MEM_TR_144:
        skip_pt = bcm56780_a0_tr144_skip_pt_list;
        count = COUNTOF(bcm56780_a0_tr144_skip_pt_list);
        break;

    case BCMA_TESTUTIL_PT_MEM_TR_143:
        bcmdrd_pt_ser_name_to_rmr(unit, "CENTRAL_EVICTION_FIFO", &bcm56780_a0_bb_skip_pt[0]);
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DOI_SLOT_PIPELINE_0", &bcm56780_a0_bb_skip_pt[1]);
        bcmdrd_pt_ser_name_to_rmr(unit, "EGR_DOI_SLOT_PIPELINE_1", &bcm56780_a0_bb_skip_pt[2]);
        bcmdrd_pt_ser_name_to_rmr(unit, "FT_LEARN_FIFO", &bcm56780_a0_bb_skip_pt[3]);
        bcmdrd_pt_ser_name_to_rmr(unit, "FT_PT_HWY_FIFO", &bcm56780_a0_bb_skip_pt[4]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_0", &bcm56780_a0_bb_skip_pt[5]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DII_EVENT_FIFO_1", &bcm56780_a0_bb_skip_pt[6]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DOI_SLOT_PIPELINE_0", &bcm56780_a0_bb_skip_pt[7]);
        bcmdrd_pt_ser_name_to_rmr(unit, "ING_DOI_SLOT_PIPELINE_1", &bcm56780_a0_bb_skip_pt[8]);
        bcmdrd_pt_ser_name_to_rmr(unit, "FT_NOTIFY_FIFO", &bcm56780_a0_bb_skip_pt[9]);
        bcmdrd_pt_ser_name_to_rmr(unit, "M_FT_NOTIFY_FIFO", &bcm56780_a0_bb_skip_pt[10]);
        bcmdrd_pt_ser_name_to_rmr(unit, "M_FT_LEARN_FIFO", &bcm56780_a0_bb_skip_pt[11]);
        skip_pt = bcm56780_a0_bb_skip_pt;
        count = COUNTOF(bcm56780_a0_bb_skip_pt);
        break;

    case BCMA_TESTUTIL_PT_MEM_TR_901:
        skip_pt = NULL;
        count = 0;
        break;

    default:
        return FALSE;
    }

    for (idx = 0; idx < count; idx++) {
        if (sid == skip_pt[idx]) {
            return TRUE;
        }
    }
    if (type == BCMA_TESTUTIL_PT_MEM_TR_144) {
        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, BLKTYPE_MMU_ITM, &inst_map);
        /*On half chip, PTs like MMU_REPL_HEAD_TBL_MMU_ITM1_ITM1m does not exist. */
        if (((sal_strstr(pt_name, "ITM1_ITM1m") != NULL) && ((inst_map & 0x2) == 0)) ||
            ((sal_strstr(pt_name, "ITM0_ITM0m") != NULL) && ((inst_map & 0x1) == 0))) {
            return TRUE;
        }
    } else if (type == BCMA_TESTUTIL_PT_MEM_DMA_RD_WR) {
        pt_name = bcmdrd_pt_sid_to_name(unit, sid);
        if (sal_strstr(pt_name, "MMU_CRB_CT_DELAY_LINE_RQE_MEMm") != NULL) {
            return TRUE;
        }
    }
    if (bcmdrd_feature_is_sim(unit) == true) {
        /* ECC table views are not supported in sim */
        skip_pt = bcm56780_a0_mem_ecc;
        count = COUNTOF(bcm56780_a0_mem_ecc);
        for (idx = 0; idx < count; idx++) {
            if (sid == skip_pt[idx]) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static int
x9_reg_cond_init(int unit, bcmdrd_sid_t sid)
{
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    UFT_PDD_LANE_MUX_CONTROLr_t pdd_lane;
    int ioerr = 0;

    SHR_FUNC_ENTER(unit);
    switch (sid) {
    case M_FT_LEARN_FAIL_COUNTERr:
    case M_FT_LEARN_CTRLr:
    case M_FT_LEARN_NOTIFY_FIFO_PTRSr:
    case M_FT_LEARN_NOTIFY_FIFO_PTRS_DEBUGr:
    case M_FT_LEARN_INTR_ENABLEr:
        /* Set lane mode to enable MTOP FT registers. */
        M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);
        M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 5);
        if (bcmdrd_pt_index_valid(unit, M_LPM_IP_CONTROLm, -1, 0)) {
            ioerr = WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
        }

        UFT_PDD_LANE_MUX_CONTROLr_CLR(pdd_lane);
        ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, pdd_lane);
        SHR_IF_ERR_EXIT(ioerr);
        break;
    default:
        break;
    }


exit:
    SHR_FUNC_EXIT();
}

static int
x9_reg_cond_cleanup(int unit, bcmdrd_sid_t sid)
{
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    int ioerr = 0;
    UFT_PDD_LANE_MUX_CONTROLr_t uft_pdd;

    SHR_FUNC_ENTER(unit);
    switch (sid) {
    case M_FT_LEARN_FAIL_COUNTERr:
    case M_FT_LEARN_CTRLr:
    case M_FT_LEARN_NOTIFY_FIFO_PTRSr:
    case M_FT_LEARN_NOTIFY_FIFO_PTRS_DEBUGr:
    case M_FT_LEARN_INTR_ENABLEr:
        /* Set lane mode back to 0. */
        M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);
        M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 0);
        if (bcmdrd_pt_index_valid(unit, M_LPM_IP_CONTROLm, -1, 0)) {
            ioerr = WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
        }
        UFT_PDD_LANE_MUX_CONTROLr_CLR(uft_pdd);
        ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, uft_pdd);
        SHR_IF_ERR_EXIT(ioerr);
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
x9_mem_cond_init(int unit, bcmdrd_sid_t sid)
{
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    int ioerr = 0 ;
    UFT_PDD_LANE_MUX_CONTROLr_t uft_pdd;

    SHR_FUNC_ENTER(unit);
    switch (sid) {
    case M_FT_GLOBAL_TABLE_CONFIGm:
    case M_FT_COMMANDm:
    case M_FT_GLOBAL_TABLE_CNTm:
    case M_FT_LEARN_NOTIFY_FIFOm:
    case M_FT_COMMAND_DATAm:
    case M_FT_GROUP_TABLE_CONFIGm:
    case M_FT_GROUP_TABLE_CNTm:
        ioerr += READ_UFT_PDD_LANE_MUX_CONTROLr(unit, &uft_pdd);
        UFT_PDD_LANE_MUX_CONTROLr_MTOP_LANE_PAIR_SELf_SET(uft_pdd, 0x0);
        ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, uft_pdd);
        M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);
        M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 5);
        if (bcmdrd_pt_index_valid(unit, M_LPM_IP_CONTROLm, -1, 0)) {
            ioerr = WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
        }
        SHR_IF_ERR_EXIT(ioerr);
        break;
    default:
        break;
    }


exit:
    SHR_FUNC_EXIT();
}

static int
x9_mem_cond_cleanup(int unit, bcmdrd_sid_t sid, bcma_testutil_pt_type_t type)
{
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    UFT_PDD_LANE_MUX_CONTROLr_t uft_pdd;
    int ioerr = 0;

    SHR_FUNC_ENTER(unit);
    switch (sid) {
    case MEMDB_TCAM_M_CTL_MEM0_0m:
    case MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m:
    case MEMDB_TCAM_M_CTL_MEM0_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_1m:
    case MEMDB_TCAM_M_CTL_MEM0_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_0m:
    case MEMDB_TCAM_M_CTL_MEM1_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_0_MEM1_1m:
    case MEMDB_TCAM_M_CTL_MEM1_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_1m:
    case MEMDB_TCAM_M_CTL_MEM1_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_0m:
    case MEMDB_TCAM_M_CTL_MEM2_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_0_MEM2_1m:
    case MEMDB_TCAM_M_CTL_MEM2_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_1m:
    case MEMDB_TCAM_M_CTL_MEM2_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_0m:
    case MEMDB_TCAM_M_CTL_MEM3_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_0_MEM3_1m:
    case MEMDB_TCAM_M_CTL_MEM3_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_1m:
    case MEMDB_TCAM_M_CTL_MEM3_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_0m:
    case MEMDB_TCAM_M_CTL_MEM4_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_0_MEM4_1m:
    case MEMDB_TCAM_M_CTL_MEM4_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_1m:
    case MEMDB_TCAM_M_CTL_MEM4_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_0m:
    case MEMDB_TCAM_M_CTL_MEM5_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_0_MEM5_1m:
    case MEMDB_TCAM_M_CTL_MEM5_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_1m:
    case MEMDB_TCAM_M_CTL_MEM5_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_0m:
    case MEMDB_TCAM_M_CTL_MEM6_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_0_MEM6_1m:
    case MEMDB_TCAM_M_CTL_MEM6_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_1m:
    case MEMDB_TCAM_M_CTL_MEM6_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_0m:
    case MEMDB_TCAM_M_CTL_MEM7_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_0_MEM7_1m:
    case MEMDB_TCAM_M_CTL_MEM7_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_1m:
    case MEMDB_TCAM_M_CTL_MEM7_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_1_ONLYm:
    case M_DEFIP_ALPM_LEVEL2_SHARED_BANKS_CONTROLm:
    case M_DEFIP_ALPM_LEVEL2_B3m:
    case M_DEFIP_ALPM_LEVEL2_B1_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B2_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4m:
    case M_DEFIP_ALPM_LEVEL2_B2m:
    case M_DEFIP_ALPM_LEVEL3_B7_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B3_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B5_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B3_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B2_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B5_ECC_2m:
    case M_DEFIP_ALPM_LEVEL2_B5m:
    case M_DEFIP_ALPM_LEVEL3_B7_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B6_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B1_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B6_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B3_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B6_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B6m:
    case M_DEFIP_ALPM_LEVEL3_B1_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B7_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B2_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B1m:
    case M_DEFIP_ALPM_LEVEL3_B4_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B7_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B6_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B5m:
    case M_DEFIP_ALPM_LEVEL2_B1_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B5_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B4m:
    case M_DEFIP_ALPM_LEVEL3_B4_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B3m:
    case M_DEFIP_ALPM_LEVEL3_B0_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0m:
    case M_DEFIP_ALPM_LEVEL2_B4_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B5_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B2_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B5_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B7m:
    case M_DEFIP_ALPM_LEVEL3_B0_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0m:
    case M_DEFIP_ALPM_LEVEL3_B2m:
    case M_DEFIP_ALPM_LEVEL2_B2_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B1m:
    case M_DEFIP_ALPM_LEVEL2_B5_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B1_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B2_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B1_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B4_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0_ECCm:
    case M_FT_GLOBAL_TABLE_CONFIGm:
    case M_FT_COMMANDm:
    case M_FT_GLOBAL_TABLE_CNTm:
    case M_FT_LEARN_NOTIFY_FIFOm:
    case M_FT_COMMAND_DATAm:
    case M_FT_GROUP_TABLE_CONFIGm:
    case M_FT_GROUP_TABLE_CNTm:
         /* Set lane mode back to 0. */
         M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);
         M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);
         if (type == BCMA_TESTUTIL_PT_MEM_TR_144) {
             M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_ECC_ENf_SET(m_lpm_ip_control,0x1);
             M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_ECC_ENf_SET(m_lpm_ip_control,0x1);
             M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
             M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
         }
         M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 0);
         if (bcmdrd_pt_index_valid(unit, M_LPM_IP_CONTROLm, -1, 0)) {
             ioerr = WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
         }
         UFT_PDD_LANE_MUX_CONTROLr_CLR(uft_pdd);
         ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, uft_pdd);
         SHR_IF_ERR_EXIT(ioerr);
         break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cond_init(int unit, bcmdrd_sid_t sid, bcma_testutil_pt_type_t type)
{

    if (type == BCMA_TESTUTIL_PT_REG_RESET ||
        type == BCMA_TESTUTIL_PT_REG_RD_WR) {
        return x9_reg_cond_init(unit, sid);
    } else {
        return x9_mem_cond_init(unit, sid);
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_cond_cleanup(int unit, bcmdrd_sid_t sid, bcma_testutil_pt_type_t type)
{

    if (type == BCMA_TESTUTIL_PT_MEM_RD_WR ||
        type == BCMA_TESTUTIL_PT_MEM_DMA_RD_WR ||
        type == BCMA_TESTUTIL_PT_MEM_TR_144 ) {
        return x9_mem_cond_cleanup(unit, sid, type);
    } else {
        return x9_reg_cond_cleanup(unit, sid);
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_pt_mask_override(int unit, bcmdrd_sid_t sid, int index,
                             uint32_t *mask, int wsize)
{
    bcmdrd_fid_t *fid_list = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    switch (sid) {
        case CDMIB_MEMm:
        {
            /*
             * For CDMIB_MEM, each row has different format.
             * And we need to skip the reserved field.
             */
            size_t num_fid, idx;
            bcmdrd_sid_t format_sid;
            const bcmdrd_sid_t format_arr[16] =
                {MIB_MEMORY_ROW0, MIB_MEMORY_ROW1, MIB_MEMORY_ROW2,
                 MIB_MEMORY_ROW3, MIB_MEMORY_ROW4, MIB_MEMORY_ROW5,
                 MIB_MEMORY_ROW6, MIB_MEMORY_ROW7, MIB_MEMORY_ROW8,
                 MIB_MEMORY_ROW9, MIB_MEMORY_ROW10, MIB_MEMORY_ROW11,
                 MIB_MEMORY_ROW12, MIB_MEMORY_ROW13, MIB_MEMORY_ROW14,
                 MIB_MEMORY_ROW15};

            format_sid = format_arr[index % 16];
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_fid_list_get(unit, format_sid, 0, NULL, &num_fid));

            SHR_ALLOC(fid_list, num_fid * sizeof(bcmdrd_fid_t),
                      "bcm56780TestUtilFidList");
            SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
            sal_memset(fid_list, 0, num_fid * sizeof(bcmdrd_fid_t));

            SHR_IF_ERR_EXIT
                (bcmdrd_pt_fid_list_get(unit, format_sid, num_fid,
                                        fid_list, &num_fid));

            sal_memset(mask, 0, wsize * sizeof(uint32_t));
            for (idx = 0; idx < num_fid; idx++) {
                int n;
                bcmdrd_sym_field_info_t finfo;

                SHR_IF_ERR_EXIT
                    (bcmdrd_pt_field_info_get(unit, format_sid, fid_list[idx],
                                              &finfo));

                if (sal_strstr(finfo.name, "RESERVED") != NULL) {
                    continue;
                }

                assert(finfo.maxbit < (wsize << 5));

                for (n = finfo.minbit; n <= finfo.maxbit; n++) {
                    int w = n / 32;
                    int b = n % 32;

                    mask[w] |= (1 << b);
                }
            }
            break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_pt_entity_override(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info,
                               bcma_testutil_pt_entity_handle_f entity_handle_cb,
                               void *user_data)
{
#define AT_DUPLICATE  9
#define AT_DUPLICATE2 15
#define AT_DUPLICATE4 16

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
            /* DUPLICATE2 is used to access pipe 0,2,4,6 */
            ibmp = ibmp & 0x55;
            break;
        case AT_DUPLICATE4:
            /* DUPLICATE4 is used to access pipe 0,4 */
            ibmp = ibmp & 0x11;
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

static void
bcm56780_a0_pkt_cfg_data_get(int unit, const bcma_testutil_pkt_lt_cfg_t **pkt_lt_cfg,
                             int *lt_cfg_num,
                             const bcma_testutil_pkt_data_t **pkt_data)
{
    *lt_cfg_num = COUNTOF(bcm56780_a0_pkt_traffic_lt_cfg);
    *pkt_lt_cfg = bcm56780_a0_pkt_traffic_lt_cfg;

    *pkt_data = &bcm56780_a0_pkt_data;
}

static int
reg_hw_update(int unit, const char *name, bool enable)
{
    bcmdrd_sid_t sid = MMU_INTFO_HW_UPDATE_DISr;
    bcmdrd_fid_t fid;
    uint32_t field_data;

    SHR_FUNC_ENTER(unit);

    field_data = enable ? 0 : 1;

    /* Disable hardware to update THDR_TO_OOBFC_SP_ST */
    fid = RQE_SPf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    /* Disable hardware to update THDO_TO_OOBFC_SP_ST */
    fid = EG_SPf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    /* Disable hardware to update THDI_TO_OOBFC_SP_ST */
    fid = ING_SPf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    /* Disable hardware to update MMU_INTFO_CONGST_ST */
    fid = CONGST_STf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    /* Disable hardware update to enable sw update MMU_INTFO_TO_XPORT_BKP */
    sid = MMU_INTFO_XPORT_BKP_HW_UPDATE_DISr;
    fid = PAUSE_PFC_BKPf;
    SHR_IF_ERR_EXIT
        (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));

    if (!enable) {
        /*
         * Deassert IDB control registers, otherwise some IDB status registers
         * will always read 0 even write non-zero value.
         */
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA_CPU_CONTROLr));
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_clear_all(unit, IDB_CA_LPBK_CONTROLr));
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
            (bcma_testutil_pt_clear_all(unit, IDB_OBM4_CONTROLr));
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

    if (sal_strcmp(name, "IPOST_LAG_LINK_STATUSm") == 0 || sal_strcmp(name, "*") == 0) {
        /* Enables status updates of LINK_STATUS via the CPU */
        field_data = enable ? 0 : 1;

        sid = IPOST_LAG_LINK_STATUS_HW_CTRLr;
        fid = SW_OVERRIDE_ENABLEf;
        SHR_IF_ERR_EXIT
            (bcma_testutil_pt_field_set(unit, sid, fid, 0, 0, field_data));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_pt_hw_update(int unit, const char *name,
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
x9_uft_init(int unit,bcma_testutil_pt_type_t type)
{
    int ioerr = 0;

    /* Hash control. */
    IFTA90_E2T_00_HASH_CONTROLm_t ifta90_e2t_00_hash_control;
    IFTA90_E2T_01_HASH_CONTROLm_t ifta90_e2t_01_hash_control;
    M_E2T_00_HASH_CONTROLm_t m_e2t_00_hash_control;
    M_E2T_01_HASH_CONTROLm_t m_e2t_01_hash_control;
    LPM_IP_CONTROLm_t lpm_ip_control;
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    UFT_PDD_LANE_MUX_CONTROLr_t uft_pdd;

    /* Shared bank control. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_t ifta90_00_sb_ctrl;
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_t ifta90_01_sb_ctrl;
    M_E2T_00_SHARED_BANKS_CONTROLm_t m_e2t_00_sb_ctrl;

    /* HASH_CONTROL table init. */
    IFTA90_E2T_00_HASH_CONTROLm_CLR(ifta90_e2t_00_hash_control);
    IFTA90_E2T_01_HASH_CONTROLm_CLR(ifta90_e2t_01_hash_control);
    M_E2T_00_HASH_CONTROLm_CLR(m_e2t_00_hash_control);
    M_E2T_01_HASH_CONTROLm_CLR(m_e2t_01_hash_control);
    LPM_IP_CONTROLm_CLR(lpm_ip_control);
    M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);

    ioerr += READ_UFT_PDD_LANE_MUX_CONTROLr(unit, &uft_pdd);
    UFT_PDD_LANE_MUX_CONTROLr_MTOP_LANE_PAIR_SELf_SET(uft_pdd, 0x0);
    ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, uft_pdd);

    ioerr += READ_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, &ifta90_e2t_00_hash_control);
    ioerr += READ_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, &ifta90_e2t_01_hash_control);
    if (bcmdrd_pt_index_valid(unit, M_E2T_00_HASH_CONTROLm, -1, 0)) {
        ioerr += READ_M_E2T_00_HASH_CONTROLm(unit, 0, &m_e2t_00_hash_control);
        ioerr += READ_M_E2T_01_HASH_CONTROLm(unit, 0, &m_e2t_01_hash_control);
        READ_M_LPM_IP_CONTROLm(unit, 0, &m_lpm_ip_control);
    }

    /* HASH_CONTROL HASH_TABLE_BANK_CONFIG. */
    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(ifta90_e2t_00_hash_control, 3);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE0f_SET(ifta90_e2t_01_hash_control, 3);
    IFTA90_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(ifta90_e2t_00_hash_control, 12);
    IFTA90_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIG_LANE1f_SET(ifta90_e2t_01_hash_control, 12);
    M_E2T_00_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(m_e2t_00_hash_control, 0xFF);
    M_E2T_01_HASH_CONTROLm_HASH_TABLE_BANK_CONFIGf_SET(m_e2t_01_hash_control, 0x00);
    M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 0);

    if (type == BCMA_TESTUTIL_PT_MEM_TR_144) {
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_ECC_ENf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_ECC_ENf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
    }
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_0f_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_1f_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_2f_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_3f_SET(m_lpm_ip_control, 0);


    /* Write to memory. */
    ioerr += WRITE_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, ifta90_e2t_00_hash_control);
    ioerr += WRITE_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, ifta90_e2t_01_hash_control);
    ioerr += WRITE_LPM_IP_CONTROLm(unit, 0, lpm_ip_control);
    if (bcmdrd_pt_index_valid(unit, M_E2T_00_HASH_CONTROLm, -1, 0)) {
        ioerr += WRITE_M_E2T_00_HASH_CONTROLm(unit, 0, m_e2t_00_hash_control);
        ioerr += WRITE_M_E2T_01_HASH_CONTROLm(unit, 0, m_e2t_01_hash_control);
        ioerr += WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
    }

    /* SHARED_BANKS_CONTROL table init. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_CLR(ifta90_00_sb_ctrl);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_CLR(ifta90_01_sb_ctrl);
    M_E2T_00_SHARED_BANKS_CONTROLm_CLR(m_e2t_00_sb_ctrl);

    /* B0_HASH_OFFSET. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0x6);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0x6);
    M_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 0x6);

    /* B1_HASH_OFFSET. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);
    M_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 0xC);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);
    M_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 18);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);
    M_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 24);

    M_E2T_00_SHARED_BANKS_CONTROLm_B4_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 30);
    M_E2T_00_SHARED_BANKS_CONTROLm_B5_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 38);
    M_E2T_00_SHARED_BANKS_CONTROLm_B6_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 44);
    M_E2T_00_SHARED_BANKS_CONTROLm_B7_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 50);

    /* Write to memory. */
    ioerr += WRITE_IFTA90_E2T_00_SHARED_BANKS_CONTROLm(unit, 0, ifta90_00_sb_ctrl);
    ioerr += WRITE_IFTA90_E2T_01_SHARED_BANKS_CONTROLm(unit, 0, ifta90_01_sb_ctrl);
    if (bcmdrd_pt_index_valid(unit, M_E2T_00_HASH_CONTROLm, -1, 0)) {
        ioerr += WRITE_M_E2T_00_SHARED_BANKS_CONTROLm(unit, 0, m_e2t_00_sb_ctrl);
    }

    return ioerr;
}

static int
x9_alpm_level3_init(int unit, bcma_testutil_pt_type_t type)
{
    int ioerr = 0;

    /* Hash control. */
    IFTA90_E2T_00_HASH_CONTROLm_t ifta90_e2t_00_hash_control;
    IFTA90_E2T_01_HASH_CONTROLm_t ifta90_e2t_01_hash_control;
    M_E2T_00_HASH_CONTROLm_t m_e2t_00_hash_control;
    M_E2T_01_HASH_CONTROLm_t m_e2t_01_hash_control;
    LPM_IP_CONTROLm_t lpm_ip_control;
    M_LPM_IP_CONTROLm_t m_lpm_ip_control;
    UFT_PDD_LANE_MUX_CONTROLr_t uft_pdd;

    /* Shared bank control. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_t ifta90_00_sb_ctrl;
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_t ifta90_01_sb_ctrl;
    M_E2T_00_SHARED_BANKS_CONTROLm_t m_e2t_00_sb_ctrl;

    /* HASH_CONTROL table init. */
    IFTA90_E2T_00_HASH_CONTROLm_CLR(ifta90_e2t_00_hash_control);
    IFTA90_E2T_01_HASH_CONTROLm_CLR(ifta90_e2t_01_hash_control);
    M_E2T_00_HASH_CONTROLm_CLR(m_e2t_00_hash_control);
    M_E2T_01_HASH_CONTROLm_CLR(m_e2t_01_hash_control);
    LPM_IP_CONTROLm_CLR(lpm_ip_control);
    M_LPM_IP_CONTROLm_CLR(m_lpm_ip_control);


    ioerr += READ_UFT_PDD_LANE_MUX_CONTROLr(unit, &uft_pdd);
    UFT_PDD_LANE_MUX_CONTROLr_MTOP_LANE_PAIR_SELf_SET(uft_pdd, 0x0);
    ioerr += WRITE_UFT_PDD_LANE_MUX_CONTROLr(unit, uft_pdd);
    ioerr += WRITE_LPM_IP_CONTROLm(unit, 0, lpm_ip_control);
    if (bcmdrd_pt_index_valid(unit, M_LPM_IP_CONTROLm, -1, 0)) {
        ioerr += WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
    }

    ioerr += READ_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, &ifta90_e2t_00_hash_control);
    ioerr += READ_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, &ifta90_e2t_01_hash_control);

    /* HASH_CONTROL HASH_TABLE_BANK_CONFIG. */
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_0f_SET(lpm_ip_control, 0x3);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_1f_SET(lpm_ip_control, 0xc);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_2f_SET(lpm_ip_control, 0x30);
    LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_3f_SET(lpm_ip_control, 0xc0);

    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_0f_SET(m_lpm_ip_control, 0x3);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_1f_SET(m_lpm_ip_control, 0xc);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_2f_SET(m_lpm_ip_control, 0x30);
    M_LPM_IP_CONTROLm_LEVEL3_BANK_CONFIG_BLOCK_3f_SET(m_lpm_ip_control, 0xc0);

    M_LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_0f_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_1f_SET(m_lpm_ip_control, 3);
    M_LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_2f_SET(m_lpm_ip_control, 6);
    M_LPM_IP_CONTROLm_LEVEL3_KEY_INPUT_SEL_BLOCK_3f_SET(m_lpm_ip_control, 9);

    M_LPM_IP_CONTROLm_LEVEL3_DATABASE_SEL_BLOCK_0f_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_LEVEL3_DATABASE_SEL_BLOCK_1f_SET(m_lpm_ip_control, 1);
    M_LPM_IP_CONTROLm_LEVEL3_DATABASE_SEL_BLOCK_2f_SET(m_lpm_ip_control, 2);
    M_LPM_IP_CONTROLm_LEVEL3_DATABASE_SEL_BLOCK_3f_SET(m_lpm_ip_control, 3);

    M_LPM_IP_CONTROLm_DB0_MODEf_SET(m_lpm_ip_control, 3);
    M_LPM_IP_CONTROLm_DB1_MODEf_SET(m_lpm_ip_control, 3);
    M_LPM_IP_CONTROLm_DB2_MODEf_SET(m_lpm_ip_control, 3);
    M_LPM_IP_CONTROLm_DB3_MODEf_SET(m_lpm_ip_control, 3);

    M_LPM_IP_CONTROLm_DATA0_SELf_SET(m_lpm_ip_control, 0);
    M_LPM_IP_CONTROLm_DATA1_SELf_SET(m_lpm_ip_control, 1);
    M_LPM_IP_CONTROLm_DATA2_SELf_SET(m_lpm_ip_control, 2);
    M_LPM_IP_CONTROLm_DATA3_SELf_SET(m_lpm_ip_control, 3);

    if (type == BCMA_TESTUTIL_PT_MEM_TR_144) {
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_ECC_ENf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_ECC_ENf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL2_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
        M_LPM_IP_CONTROLm_M_DEFIP_ALPM_LEVEL3_EN_COR_ERR_RPTf_SET(m_lpm_ip_control,0x1);
    }
    /* Write to memory. */
    ioerr += WRITE_IFTA90_E2T_00_HASH_CONTROLm(unit, 0, ifta90_e2t_00_hash_control);
    ioerr += WRITE_IFTA90_E2T_01_HASH_CONTROLm(unit, 0, ifta90_e2t_01_hash_control);
    ioerr += WRITE_LPM_IP_CONTROLm(unit, 0, lpm_ip_control);
    if (bcmdrd_pt_index_valid(unit, M_E2T_00_HASH_CONTROLm, -1, 0)) {
        ioerr += WRITE_M_E2T_00_HASH_CONTROLm(unit, 0, m_e2t_00_hash_control);
        ioerr += WRITE_M_E2T_01_HASH_CONTROLm(unit, 0, m_e2t_01_hash_control);
    }

    /* SHARED_BANKS_CONTROL table init. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_CLR(ifta90_00_sb_ctrl);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_CLR(ifta90_01_sb_ctrl);
    M_E2T_00_SHARED_BANKS_CONTROLm_CLR(m_e2t_00_sb_ctrl);

    /* BANK_BYPASS_LP. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(ifta90_00_sb_ctrl, 0x3);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(ifta90_01_sb_ctrl, 0x3);
    M_E2T_00_SHARED_BANKS_CONTROLm_BANK_BYPASS_LPf_SET(m_e2t_00_sb_ctrl, 0xFF);

    /* B0_HASH_OFFSET. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0x6);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0x6);
    M_E2T_00_SHARED_BANKS_CONTROLm_B0_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 0x6);

    /* B1_HASH_OFFSET. */
    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);

    IFTA90_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(ifta90_00_sb_ctrl, 0xC);
    IFTA90_E2T_01_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(ifta90_01_sb_ctrl, 0xC);

    M_E2T_00_SHARED_BANKS_CONTROLm_B1_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 12);
    M_E2T_00_SHARED_BANKS_CONTROLm_B2_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 28);
    M_E2T_00_SHARED_BANKS_CONTROLm_B3_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 24);
    M_E2T_00_SHARED_BANKS_CONTROLm_B4_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 30);
    M_E2T_00_SHARED_BANKS_CONTROLm_B5_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 38);
    M_E2T_00_SHARED_BANKS_CONTROLm_B6_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 44);
    M_E2T_00_SHARED_BANKS_CONTROLm_B7_HASH_OFFSETf_SET(m_e2t_00_sb_ctrl, 50);


    /* Write to memory. */
    ioerr += WRITE_IFTA90_E2T_00_SHARED_BANKS_CONTROLm(unit, 0, ifta90_00_sb_ctrl);
    ioerr += WRITE_IFTA90_E2T_01_SHARED_BANKS_CONTROLm(unit, 0, ifta90_01_sb_ctrl);

    if (bcmdrd_pt_index_valid(unit, M_E2T_00_HASH_CONTROLm, -1, 0)) {
        M_LPM_IP_CONTROLm_M_LANE_MODEf_SET(m_lpm_ip_control, 1);
        ioerr += WRITE_M_E2T_00_SHARED_BANKS_CONTROLm(unit, 0, m_e2t_00_sb_ctrl);
        ioerr += WRITE_M_LPM_IP_CONTROLm(unit, 0, m_lpm_ip_control);
    }

    return ioerr;
}

static int
bcm56780_a0_mem_address_adjust(int unit, bcmdrd_sid_t sid,
                               int *index_start, int *index_end,
                               bcma_testutil_pt_type_t type)
{
    SHR_FUNC_ENTER(unit);

    switch (sid) {
    case IFTA90_E2T_00_B0_ECCm:
    case IFTA90_E2T_00_B0_SINGLEm:
    case IFTA90_E2T_00_B0_DOUBLEm:
    case IFTA90_E2T_00_B0_QUADm:
    case IFTA90_E2T_00_B1_ECCm:
    case IFTA90_E2T_00_B1_SINGLEm:
    case IFTA90_E2T_00_B1_DOUBLEm:
    case IFTA90_E2T_00_B1_QUADm:
    case IFTA90_E2T_00_B2_ECCm:
    case IFTA90_E2T_00_B2_SINGLEm:
    case IFTA90_E2T_00_B2_DOUBLEm:
    case IFTA90_E2T_00_B2_QUADm:
    case IFTA90_E2T_00_B3_ECCm:
    case IFTA90_E2T_00_B3_SINGLEm:
    case IFTA90_E2T_00_B3_DOUBLEm:
    case IFTA90_E2T_00_B3_QUADm:
    case IFTA90_E2T_01_B0_ECCm:
    case IFTA90_E2T_01_B0_SINGLEm:
    case IFTA90_E2T_01_B0_DOUBLEm:
    case IFTA90_E2T_01_B0_QUADm:
    case IFTA90_E2T_01_B1_ECCm:
    case IFTA90_E2T_01_B1_SINGLEm:
    case IFTA90_E2T_01_B1_DOUBLEm:
    case IFTA90_E2T_01_B1_QUADm:
    case IFTA90_E2T_01_B2_ECCm:
    case IFTA90_E2T_01_B2_SINGLEm:
    case IFTA90_E2T_01_B2_DOUBLEm:
    case IFTA90_E2T_01_B2_QUADm:
    case IFTA90_E2T_01_B3_ECCm:
    case IFTA90_E2T_01_B3_SINGLEm:
    case IFTA90_E2T_01_B3_DOUBLEm:
    case IFTA90_E2T_01_B3_QUADm:
    case M_E2T_B0_DOUBLEm:
    case M_E2T_B4_SINGLE_2m:
    case M_E2T_B1_QUADm:
    case M_E2T_B5_SINGLEm:
    case M_E2T_B6_DOUBLEm:
    case M_E2T_B3_DOUBLEm:
    case M_E2T_B1_DOUBLEm:
    case M_E2T_B7_SINGLE_2m:
    case M_E2T_B0_QUADm:
    case M_E2T_B2_SINGLE_2m:
    case M_E2T_B7_DOUBLEm:
    case M_E2T_B2_QUADm:
    case M_E2T_B4_SINGLEm:
    case M_E2T_B4_QUADm:
    case M_E2T_B5_DOUBLEm:
    case M_E2T_B2_DOUBLEm:
    case M_E2T_B6_ECCm:
    case M_E2T_B6_SINGLEm:
    case M_E2T_B0_ECCm:
    case M_E2T_B7_ECC_2m:
    case M_E2T_B7_ECCm:
    case M_E2T_B5_ECCm:
    case M_E2T_B1_SINGLEm:
    case M_E2T_B6_ECC_2m:
    case M_E2T_B7_SINGLEm:
    case M_E2T_B0_SINGLEm:
    case M_E2T_B1_ECCm:
    case M_E2T_B3_SINGLE_2m:
    case M_E2T_B5_SINGLE_2m:
    case M_E2T_B7_QUADm:
    case M_E2T_B1_SINGLE_2m:
    case M_E2T_B3_SINGLEm:
    case M_E2T_B4_ECCm:
    case M_E2T_B0_ECC_2m:
    case M_E2T_B1_ECC_2m:
    case M_E2T_B4_ECC_2m:
    case M_E2T_B6_QUADm:
    case M_E2T_B2_ECC_2m:
    case M_E2T_B0_SINGLE_2m:
    case M_E2T_B2_ECCm:
    case M_E2T_B3_QUADm:
    case M_E2T_B3_ECC_2m:
    case M_E2T_B5_ECC_2m:
    case M_E2T_B3_ECCm:
    case M_E2T_B5_QUADm:
    case M_E2T_B6_SINGLE_2m:
    case M_E2T_B2_SINGLEm:
    case M_E2T_B4_DOUBLEm:
    case M_E2T_B0_LPm:
    case M_E2T_B1_LPm:
    case M_E2T_B2_LPm:
    case M_E2T_B3_LPm:
    case M_E2T_B4_LPm:
    case M_E2T_B5_LPm:
    case M_E2T_B6_LPm:
    case M_E2T_B7_LPm:
    case M_E2T_00_ACTION_TABLE_Am:
    case M_E2T_00_ACTION_TABLE_Bm:
    case M_E2T_00_REMAP_TABLE_Am:
    case M_E2T_00_REMAP_TABLE_Bm:
    case M_E2T_01_ACTION_TABLE_Am:
    case M_E2T_01_ACTION_TABLE_Bm:
    case M_E2T_01_REMAP_TABLE_Am:
    case M_E2T_01_REMAP_TABLE_Bm:
        SHR_IF_ERR_EXIT
            (x9_uft_init(unit,type));
        break;

    case L3_DEFIP_ALPM_LEVEL3_B0m:
    case L3_DEFIP_ALPM_LEVEL3_B0_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B0_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B1m:
    case L3_DEFIP_ALPM_LEVEL3_B1_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B1_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B2m:
    case L3_DEFIP_ALPM_LEVEL3_B2_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B2_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B3m:
    case L3_DEFIP_ALPM_LEVEL3_B3_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B3_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B4m:
    case L3_DEFIP_ALPM_LEVEL3_B4_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B4_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B5m:
    case L3_DEFIP_ALPM_LEVEL3_B5_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B5_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B6m:
    case L3_DEFIP_ALPM_LEVEL3_B6_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B6_SINGLEm:
    case L3_DEFIP_ALPM_LEVEL3_B7m:
    case L3_DEFIP_ALPM_LEVEL3_B7_ECCm:
    case L3_DEFIP_ALPM_LEVEL3_B7_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_SHARED_BANKS_CONTROLm:
    case M_DEFIP_ALPM_LEVEL2_B3m:
    case M_DEFIP_ALPM_LEVEL2_B1_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B2_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4m:
    case M_DEFIP_ALPM_LEVEL2_B2m:
    case M_DEFIP_ALPM_LEVEL3_B7_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B3_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B5_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B3_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B2_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B5_ECC_2m:
    case M_DEFIP_ALPM_LEVEL2_B5m:
    case M_DEFIP_ALPM_LEVEL3_B7_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B6_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B1_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B6_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B3_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B6_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B6m:
    case M_DEFIP_ALPM_LEVEL3_B1_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B7_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B2_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B1m:
    case M_DEFIP_ALPM_LEVEL3_B4_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B3_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B7_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B6_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B5m:
    case M_DEFIP_ALPM_LEVEL2_B1_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B5_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B4_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B4m:
    case M_DEFIP_ALPM_LEVEL3_B4_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B3m:
    case M_DEFIP_ALPM_LEVEL3_B0_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B0m:
    case M_DEFIP_ALPM_LEVEL2_B4_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B5_ECCm:
    case M_DEFIP_ALPM_LEVEL3_B2_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B5_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B7m:
    case M_DEFIP_ALPM_LEVEL3_B0_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0m:
    case M_DEFIP_ALPM_LEVEL3_B2m:
    case M_DEFIP_ALPM_LEVEL2_B2_ECCm:
    case M_DEFIP_ALPM_LEVEL2_B1m:
    case M_DEFIP_ALPM_LEVEL2_B5_SINGLEm:
    case M_DEFIP_ALPM_LEVEL3_B1_SINGLE_2m:
    case M_DEFIP_ALPM_LEVEL3_B2_ECC_2m:
    case M_DEFIP_ALPM_LEVEL3_B1_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B4_SINGLEm:
    case M_DEFIP_ALPM_LEVEL2_B0_ECCm:
        SHR_IF_ERR_EXIT
            (x9_alpm_level3_init(unit,type));
        break;

    case MEMDB_TCAM_M_CTL_MEM0_0m:
    case MEMDB_TCAM_M_CTL_MEM0_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_0_MEM0_1m:
    case MEMDB_TCAM_M_CTL_MEM0_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_1m:
    case MEMDB_TCAM_M_CTL_MEM0_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM0_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_0m:
    case MEMDB_TCAM_M_CTL_MEM1_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_0_MEM1_1m:
    case MEMDB_TCAM_M_CTL_MEM1_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_1m:
    case MEMDB_TCAM_M_CTL_MEM1_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM1_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_0m:
    case MEMDB_TCAM_M_CTL_MEM2_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_0_MEM2_1m:
    case MEMDB_TCAM_M_CTL_MEM2_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_1m:
    case MEMDB_TCAM_M_CTL_MEM2_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM2_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_0m:
    case MEMDB_TCAM_M_CTL_MEM3_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_0_MEM3_1m:
    case MEMDB_TCAM_M_CTL_MEM3_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_1m:
    case MEMDB_TCAM_M_CTL_MEM3_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM3_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_0m:
    case MEMDB_TCAM_M_CTL_MEM4_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_0_MEM4_1m:
    case MEMDB_TCAM_M_CTL_MEM4_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_1m:
    case MEMDB_TCAM_M_CTL_MEM4_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM4_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_0m:
    case MEMDB_TCAM_M_CTL_MEM5_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_0_MEM5_1m:
    case MEMDB_TCAM_M_CTL_MEM5_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_1m:
    case MEMDB_TCAM_M_CTL_MEM5_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM5_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_0m:
    case MEMDB_TCAM_M_CTL_MEM6_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_0_MEM6_1m:
    case MEMDB_TCAM_M_CTL_MEM6_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_1m:
    case MEMDB_TCAM_M_CTL_MEM6_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM6_1_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_0m:
    case MEMDB_TCAM_M_CTL_MEM7_0_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_0_MEM7_1m:
    case MEMDB_TCAM_M_CTL_MEM7_0_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_1m:
    case MEMDB_TCAM_M_CTL_MEM7_1_DATA_ONLYm:
    case MEMDB_TCAM_M_CTL_MEM7_1_ONLYm:
        SHR_IF_ERR_EXIT
            (x9_alpm_level3_init(unit,type));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cmc_chan_get(int unit, int *cmc_num, int *cmc_chan_num)
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
bcm56780_a0_chip_blktype_get(int unit, const char *blktype_name,
                             int *blktype_id)
{
    int count = COUNTOF(bcm56780_a0_blktype_name_id_map), block;

    block = bcma_testutil_name2id(bcm56780_a0_blktype_name_id_map,
                                  count, blktype_name);

    *blktype_id = block;

    return SHR_E_NONE;
}

static const bcmdrd_sid_t bcm56780_a0_becchmark_reg[] = {
    ING_DOP_CTRL_0_64r, /* socreg */
    CMIC_RPE_PKT_VLANr  /* cmicreg */
};

static const bcmdrd_sid_t bcm56780_a0_becchmark_mem[] = {
    /* not support lookup/insert/delete in PT, so skip hash type memory */
    IPARSER2_HME_INIT_PROFILEm, /* index */
    FLEX_DIGEST_LKUP_NET_LAYER_TCAM_0_ONLYm,    /* tcam */
};

static int
bcm56780_a0_benchmark_reglist_get(int unit, const bcmdrd_sid_t **reg_arr,
                                  int *reg_cnt)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(reg_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(reg_cnt, SHR_E_PARAM);

    *reg_arr = bcm56780_a0_becchmark_reg;
    *reg_cnt = COUNTOF(bcm56780_a0_becchmark_reg);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_benchmark_memlist_get(int unit, const bcmdrd_sid_t **mem_arr,
                                  int *mem_cnt)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(mem_cnt, SHR_E_PARAM);

    *mem_arr = bcm56780_a0_becchmark_mem;
    *mem_cnt = COUNTOF(bcm56780_a0_becchmark_mem);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_mem_cambist_info(int unit, bcmdrd_sid_t sid, bool *is_bist,
                             bcma_testutil_mem_cambist_entry_t *info)
{
#define CAMBIST_STATUS_EXPECTED (0x1f)

    const char *control_name = bcmdrd_pt_sid_to_name(unit, sid);
    char result_name[BCMA_TESTUTIL_STR_MAX_SIZE];
    const char *control_postfix = "BIST_CONFIG";
    const char *result_postfix  = "BIST_STATUSr";
    char *result_prefix;
    bcmdrd_fid_t *fields_list = NULL;
    size_t fields_num, i, prefix_len;

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
         * For TD4, there should be only one enable/reset/mode field
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

            if (sal_strstr(control_name, "2T_")) {
                info->control_field_enable_arr_count[0] = 2;
            } else if (sal_strstr(control_name, "4T_")) {
                info->control_field_enable_arr_count[0] = 4;
            } else if (sal_strstr(control_name, "EDIT_CTRL_TCAM")) {
                info->control_field_enable_arr_count[0] = 15;
            } else if (sal_strstr(control_name, "FLEX_DIGEST_LKUP")) {
                info->control_field_enable_arr_count[0] = 3;
            } else if (sal_strstr(control_name, "IPARSER1_HME_TCAM_64X192_DTOP_CONTROLLER_0")) {
                info->control_field_enable_arr_count[0] = 6;
            } else if (sal_strstr(control_name, "IPARSER2_HME_TCAM_64X192_DTOP_CONTROLLER_0")) {
                info->control_field_enable_arr_count[0] = 2;
            } else if (sal_strstr(control_name, "IPARSER2_HME_TCAM_64X192_DTOP_CONTROLLER_1")) {
                info->control_field_enable_arr_count[0] = 3;
            } else {
                info->control_field_enable_arr_count[0] = 1;
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
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL); /* enable field is must have */
    }

    /*
     * find the corresponding status register name
     *   ex.    CPU_COS_CAM_BIST_CONFIG
     *      ==> CPU_COS_CAM_BIST_STATUS
     */
    if (sal_strlen(control_name) + 1 > BCMA_TESTUTIL_STR_MAX_SIZE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(result_name, control_name, sal_strlen(control_name) + 1);
    result_prefix = sal_strstr(result_name, control_postfix);
    if (result_prefix == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    prefix_len = sal_strlen(result_name) - sal_strlen(result_prefix);
    if (prefix_len + sal_strlen(result_postfix) + 1
        > BCMA_TESTUTIL_STR_MAX_SIZE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memcpy(result_prefix, result_postfix, sal_strlen(result_postfix) + 1);

    if (SHR_FAILURE
            (bcmdrd_pt_name_to_sid(unit, result_name, &info->result_reg))) {
         LOG_ERROR(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                               "cannot find status = %s from %s\
                                (sid = %"PRId32")\n"),
                    result_name, control_name, info->control_reg));
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


bcma_testutil_drv_power_op_t bcm56780_a0_power_op = {
    .power_help         = bcm56780_a0_power_help,
    .power_init         = bcm56780_a0_power_init,
    .power_port_set     = bcm56780_a0_power_port_set,
    .power_config_set   = bcm56780_a0_power_config_set,
    .power_txrx         = bcm56780_a0_power_txrx
};

int
bcm56780_a0_testutil_drv_attach(int unit)
{
    bcma_testutil_drv_t* testutil_drv;

    testutil_drv = bcma_testutil_drv_get(unit);
    if (testutil_drv == NULL) {
        return SHR_E_FAIL;
    }

    testutil_drv->pt_skip = bcm56780_a0_pt_skip;
    testutil_drv->pt_hw_update = bcm56780_a0_pt_hw_update;
    testutil_drv->pt_mask_override = bcm56780_a0_pt_mask_override;
    testutil_drv->pt_entity_override = bcm56780_a0_pt_entity_override;
    testutil_drv->mem_address_adjust = bcm56780_a0_mem_address_adjust;
    testutil_drv->mem_range_write = bcma_testutil_cmicx_mem_range_write;
    testutil_drv->mem_range_read = bcma_testutil_cmicx_mem_range_read;
    testutil_drv->mem_mbist_info = bcm56780_a0_mem_mbist_info;
    testutil_drv->mem_mbist_dma_info = bcm56780_a0_mem_mbist_dma_info;
    testutil_drv->cmc_chan_get = bcm56780_a0_cmc_chan_get;
    testutil_drv->benchmark_reglist_get = bcm56780_a0_benchmark_reglist_get;
    testutil_drv->benchmark_memlist_get = bcm56780_a0_benchmark_memlist_get;
    testutil_drv->mem_cambist_info= bcm56780_a0_mem_cambist_info;
    testutil_drv->vlan_op_get = bcma_testutil_xfs_vlan_op_get;
    testutil_drv->l2_op_get = bcma_testutil_xfs_l2_op_get;
    testutil_drv->ctr_op_get = bcma_testutil_queue_ctr_op_get;
    testutil_drv->chip_blktype_get = bcm56780_a0_chip_blktype_get;
    testutil_drv->check_health = bcm56780_a0_check_health;
    testutil_drv->pkt_cfg_data_get = bcm56780_a0_pkt_cfg_data_get;
    testutil_drv->flex_lt_op_get = bcma_testutil_xfs_flex_lt_op_get;
    testutil_drv->flex_traffic_op_get = bcm56780_a0_flex_traffic_op_get;
    testutil_drv->traffic_expeceted_rate_get = bcm56780_a0_traffic_get_expeceted_rate;
    testutil_drv->traffic_fixed_packet_list_get = bcm56780_a0_traffic_get_fixed_packet_list;
    testutil_drv->traffic_random_packet_list_get = bcm56780_a0_traffic_get_random_packet_list;
    testutil_drv->traffic_worstcase_pktlen_get = bcm56780_a0_traffic_get_worstcase_pktlen;
    testutil_drv->power_op_get = &bcm56780_a0_power_op;
    testutil_drv->pt_write = bcmbd_pt_write;
    testutil_drv->pt_read = bcmbd_pt_read;
    testutil_drv->qos_lt_op_get = bcma_testutil_xfs_qos_lt_op_get;
    testutil_drv->cond_init = bcm56780_a0_cond_init;
    testutil_drv->cond_cleanup = bcm56780_a0_cond_cleanup;
    testutil_drv->pt_default_value_get = bcmdrd_pt_default_value_get;

    SHR_BITSET(testutil_drv->features,
               BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD);
    SHR_BITSET(testutil_drv->features,
               BCMA_TESTUTIL_FT_NEED_NOT_TDM_CODING_ALGO);
    SHR_BITSET(testutil_drv->features,
               BCMA_TESTUTIL_FT_NOT_SUPPORT_PT_LOOKUP);
    return 0;
}
