/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This test is intended to be used for in house SER testing. The test writes
 * all SW accessible memories on chip and reads them back to check for flips.
 * The assumption is that this test will be run at regular intervals in a loop
 * that is part of a master soc script as per test requirements. The test
 * provides information on the total memory tested, total number of flips and
 * the logical coordinate of each flip. However it does not provide information
 * about the physical coordinate of each flip.
 *
 * Configuration parameters passed from CLI:
 * TestPat: Test Pattern used to fill the memories
 *          0: All 0s
 *          1: All 1s
 *          2: Logical checker board
 *          3: Logical inverse checker board
 * RdWrMode: Whether the test writes or reads the memories
 *           1: Test only writes all the memories under test
 *           2: Test only reads back all memories under test and counts flips
 */

#include <appl/diag/system.h>
#include <shared/alloc.h>
#include <sal/core/alloc.h>
#include <shared/bsl.h>
#include<soc/mcm/intenum.h>

#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/cmicm.h>
#include <soc/cmic.h>

#include <sal/types.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>

#include "testlist.h"

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)


#define MEM_WORD 32
#define BYTE 8
#define CHB_FIRST_WORD 0x55555555
#define ICHB_FIRST_WORD 0xaaaaaaaa

#define MAX_FAIL_COUNT 10000
#define TEST_PATTERN_ALL0 0
#define TEST_PATTERN_ALL1 1
#define TEST_PATTERN_CHB 2
#define TEST_PATTERN_ICHB 3

#define WRITE_ONLY 1
#define READ_ONLY 2

#define TEST_PATTERN_PARAM_DEFAULT 2
#define RD_WR_MODE_PARAM_DEFAULT 1
#define QUIET_MODE_PARAM_DEFAULT 0

#define TH_ACC_TYPE_DUPLICATE 9
#define TH_ACC_TYPE_DATA_SPLIT 14
#define TH_ACC_TYPE_UNIQUE_PIPE0 0
#define TH_ACC_TYPE_UNIQUE_PIPE1 1
#define TH_ACC_TYPE_UNIQUE_PIPE2 2
#define TH_ACC_TYPE_UNIQUE_PIPE3 3
#define TH_ACC_TYPE_SINGLE 20
#define TH_ACC_TYPE_ADDR_SPLIT_DIST 10
#define TH_ACC_TYPE_ADDR_SPLIT_SPLIT 12
#define TH_NUM_UNIQUE_ACC_TYPES 4
#define TH_NUM_VALID_ACC_TYPES 9
#define TH_BASE_TYPE_IPORT 0
#define TH_BASE_TYPE_EPORT 1
#define TH_BASE_TYPE_IPIPE 2
#define TH_BASE_TYPE_EPIPE 3
#define TH_BASE_TYPE_CHIP 4
#define TH_BASE_TYPE_XPE 5
#define TH_BASE_TYPE_SLICE 6
#define TH_BASE_TYPE_LAYER 7
#define TH_NUM_VALID_BASE_TYPES 8
#define SBUS_V4_MMU_MEMIDX_MASK 0x0003ffff
#define TH_NUM_BASE_TYPE_BLOCKS 3
#define TH_MMU_ACC_TYPE_BOUNDARY 32768
#define INVALID_ACC_TYPE 999

typedef void (*get_valid_unique_acc_types_t) (int, soc_mem_t, uint32*);

typedef struct ser_ih_s {
    uint32 test_pattern_param;
    uint32 rd_wr_mode_param;
    uint32 quiet_mode_param;
    uint32 total_memories_written;
    uint32 total_memories_read;
    uint32 total_bits_tested;
    uint32 bad_input;
    uint32 total_flips;
    uint32 test_fail;
    uint32 num_acc_types;
    uint32 num_unique_acc_types;
    uint32 acc_type_duplicate;
    uint32 acc_type_data_split;
    uint32 acc_type_single;
    uint32 acc_type_addr_split_dist;
    uint32 acc_type_addr_split_split;
    uint32 *acc_type_unique;
    uint32 sbus_v4;
    uint32 num_base_type_blocks;
    get_valid_unique_acc_types_t get_valid_unique_acc_types;
    uint32 memidx_mask;
    uint32 **mask_array;
} ser_ih_t;

static ser_ih_t *ser_ih_parray[SOC_MAX_NUM_DEVICES];
/* The do_not_test_list array contains a list of memories not to be tested. This
   includes memories that are updated continuously by hardware even this test is
   ran immediately after "init soc". The 'rc.soc" is not executed as it starts
   more software threads and keeps writing to more memories. The list also
   includes overlays and aggregates that are missed in the soc_mem_info_t. */
#ifdef BCM_TOMAHAWK2_SUPPORT
static soc_mem_t th2_do_not_test_list[] = {
                        DLB_ECMP_GROUP_CONTROLm,
                        DLB_ECMP_PORT_INST_QUALITY_MEASUREm,
                        DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE0m,
                        DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE1m,
                        DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE2m,
                        DLB_ECMP_PORT_INST_QUALITY_MEASURE_PIPE3m,
                        DLB_HGT_LAG_GROUP_CONTROLm,
                        DLB_HGT_LAG_PORT_INST_QUALITY_MEASUREm,
                        DLB_HGT_LAG_PORT_INST_QUALITY_MEASURE_PIPE0m,
                        DLB_HGT_LAG_PORT_INST_QUALITY_MEASURE_PIPE1m,
                        DLB_HGT_LAG_PORT_INST_QUALITY_MEASURE_PIPE2m,
                        DLB_HGT_LAG_PORT_INST_QUALITY_MEASURE_PIPE3m,
                        EFP_COUNTER_TABLEm,
                        EFP_COUNTER_TABLE_PIPE0m,
                        EFP_COUNTER_TABLE_PIPE1m,
                        EFP_COUNTER_TABLE_PIPE2m,
                        EFP_COUNTER_TABLE_PIPE3m,
                        EFP_METER_TABLEm,
                        EFP_METER_TABLE_PIPE0m,
                        EFP_METER_TABLE_PIPE1m,
                        EFP_METER_TABLE_PIPE2m,
                        EFP_METER_TABLE_PIPE3m,
                        EGR_PERQ_XMT_COUNTERSm,
                        EGR_PERQ_XMT_COUNTERS_PIPE0m,
                        EGR_PERQ_XMT_COUNTERS_PIPE1m,
                        EGR_PERQ_XMT_COUNTERS_PIPE2m,
                        EGR_PERQ_XMT_COUNTERS_PIPE3m,
                        EXACT_MATCH_4m,
                        EXACT_MATCH_2_ENTRY_ONLYm,
                        EXACT_MATCH_2_ENTRY_ONLY_PIPE0m,
                        EXACT_MATCH_2_ENTRY_ONLY_PIPE1m,
                        EXACT_MATCH_2_ENTRY_ONLY_PIPE2m,
                        EXACT_MATCH_2_ENTRY_ONLY_PIPE3m,
                        EXACT_MATCH_2_PIPE0m,
                        EXACT_MATCH_2_PIPE1m,
                        EXACT_MATCH_2_PIPE2m,
                        EXACT_MATCH_2_PIPE3m,
                        EXACT_MATCH_4_ENTRY_ONLYm,
                        EXACT_MATCH_4_ENTRY_ONLY_PIPE0m,
                        EXACT_MATCH_4_ENTRY_ONLY_PIPE1m,
                        EXACT_MATCH_4_ENTRY_ONLY_PIPE2m,
                        EXACT_MATCH_4_ENTRY_ONLY_PIPE3m,
                        EXACT_MATCH_4_PIPE0m,
                        EXACT_MATCH_4_PIPE1m,
                        EXACT_MATCH_4_PIPE2m,
                        EXACT_MATCH_4_PIPE3m,
                        EXACT_MATCH_HIT_ONLYm,
                        FPEM_ECCm,
                        FPEM_ECC_PIPE0m,
                        FPEM_ECC_PIPE1m,
                        FPEM_ECC_PIPE2m,
                        FPEM_ECC_PIPE3m,
                        IFP_METER_TABLEm,
                        IFP_METER_TABLE_PIPE0m,
                        IFP_METER_TABLE_PIPE1m,
                        IFP_METER_TABLE_PIPE2m,
                        IFP_METER_TABLE_PIPE3m,
                        IFP_STORM_CONTROL_METERSm,
                        L3_ENTRY_ONLYm,
                        MMU_MTRO_BUCKET_L0_MEMm,
                        MMU_MTRO_BUCKET_L0_MEM_0m,
                        MMU_MTRO_BUCKET_L0_MEM_1m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE0m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE1m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE2m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE3m,
                        MMU_MTRO_BUCKET_L1_MEMm,
                        MMU_MTRO_BUCKET_L1_MEM_0m,
                        MMU_MTRO_BUCKET_L1_MEM_1m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE0m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE1m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE2m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE3m,
                        MMU_MTRO_BUCKET_L2_MEMm,
                        MMU_MTRO_BUCKET_L2_MEM_0m,
                        MMU_MTRO_BUCKET_L2_MEM_1m,
                        MMU_MTRO_BUCKET_L2_MEM_2m,
                        MMU_MTRO_BUCKET_L2_MEM_3m,
                        MMU_MTRO_BUCKET_L2_MEM_4m,
                        MMU_MTRO_BUCKET_L2_MEM_5m,
                        MMU_MTRO_BUCKET_L2_MEM_6m,
                        MMU_MTRO_BUCKET_L2_MEM_7m,
                        MMU_MTRO_BUCKET_S1_MEM_0m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEMm,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_0m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_1m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE0m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE1m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE2m,
                        MMU_MTRO_EGRMETERINGBUCKET_MEM_PIPE3m,
                        MMU_SEDCFG_MEM_FAIL_ADDR_64m,
                        MMU_SEDCFG_MEM_FAIL_ADDR_64_SC0m,
                        MMU_SEDCFG_MEM_FAIL_ADDR_64_SC1m,
                        MMU_THDU_UCQ_STATS_TABLEm,
                        MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE0m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE0_PIPE1m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE1_PIPE2m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE1_PIPE3m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE2_PIPE0m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE2_PIPE1m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE3_PIPE2m,
                        MMU_THDU_UCQ_STATS_TABLE_XPE3_PIPE3m,
                        MMU_WRED_AVG_QSIZE_XPE0_PIPE0m,
                        MMU_WRED_AVG_QSIZE_XPE0_PIPE1m,
                        MMU_WRED_AVG_QSIZE_XPE1_PIPE2m,
                        MMU_WRED_AVG_QSIZE_XPE1_PIPE3m,
                        MMU_WRED_AVG_QSIZE_XPE2_PIPE0m,
                        MMU_WRED_AVG_QSIZE_XPE2_PIPE1m,
                        MMU_WRED_AVG_QSIZE_XPE3_PIPE2m,
                        MMU_WRED_AVG_QSIZE_XPE3_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE0_PIPE0m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE0_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE2_PIPE0m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE2_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE0_PIPE0m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE0_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE2_PIPE0m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE2_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE2_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE2_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE2_PIPE0m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE3m
};
#else
static soc_mem_t th2_do_not_test_list[1];
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
static soc_mem_t th_do_not_test_list[] = {
                        EGR_SER_FIFOm,
                        EGR_SER_FIFO_PIPE0m,
                        EGR_SER_FIFO_PIPE1m,
                        EGR_SER_FIFO_PIPE2m,
                        EGR_SER_FIFO_PIPE3m,
                        ING_SER_FIFOm,
                        ING_SER_FIFO_PIPE0m,
                        ING_SER_FIFO_PIPE1m,
                        ING_SER_FIFO_PIPE2m,
                        ING_SER_FIFO_PIPE3m,
                        L2_MGMT_SER_FIFOm,
                        SER_ACC_TYPE_MAPm,
                        SER_MEMORYm,
                        SER_RESULT_0m,
                        SER_RESULT_1m,
                        SER_RESULT_DATA_0m,
                        SER_RESULT_DATA_1m,
                        SER_RESULT_EXPECTED_0m,
                        SER_RESULT_EXPECTED_1m,
                        CENTRAL_CTR_EVICTION_FIFOm,
                        CLPORT_WC_UCMEM_DATAm,
                        XLPORT_WC_UCMEM_DATAm,
                        L2Xm,
                        L2_BULKm,
                        L2_ENTRY_ISS_LPm,
                        L2_ENTRY_ONLY_TILEm,
                        L3_DEFIP_AUX_HITBIT_UPDATEm,
                        L3_DEFIP_PAIR_128_DATA_ONLYm,
                        L3_DEFIP_PAIR_128_HIT_ONLYm,
                        L3_DEFIP_DATA_ONLYm,
                        L3_ENTRY_HIT_ONLYm,
                        L3_ENTRY_IPV4_MULTICASTm,
                        L3_ENTRY_IPV4_UNICASTm,
                        L3_ENTRY_IPV6_MULTICASTm,
                        L3_ENTRY_IPV6_UNICASTm,
                        L3_ENTRY_LPm,
                        MMU_GCFG_MEM_FAIL_ADDR_64m,
                        MMU_SCFG_MEM_FAIL_ADDR_64_SC0m,
                        MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m,
                        MMU_XCFG_MEM_FAIL_ADDR_64_XPE1m,
                        MMU_XCFG_MEM_FAIL_ADDR_64_XPE2m,
                        MMU_XCFG_MEM_FAIL_ADDR_64_XPE3m,
                        MMU_SCFG_MEM_FAIL_ADDR_64_SC1m,
                        EGR_IP_TUNNEL_IPV6m,
                        EGR_IP_TUNNEL_MPLSm,
                        EDB_1DBG_Bm,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE0m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE1m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE2m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE3m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE0m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE1m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE2m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE3m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
                        THDI_PORT_SP_CONFIG_PIPE0m,
                        THDI_PORT_SP_CONFIG_PIPE1m,
                        THDI_PORT_SP_CONFIG_PIPE2m,
                        THDI_PORT_SP_CONFIG_PIPE3m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE0m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE1m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE2m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE3m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE0m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE1m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE2m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE3m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE0m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE1m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE2m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE3m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE0m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE1m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE2m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE3m,
                        MMU_THDU_CONFIG_QUEUE_PIPE0m,
                        MMU_THDU_CONFIG_QUEUE_PIPE1m,
                        MMU_THDU_CONFIG_QUEUE_PIPE2m,
                        MMU_THDU_CONFIG_QUEUE_PIPE3m,
                        MMU_THDU_OFFSET_QUEUE_PIPE0m,
                        MMU_THDU_OFFSET_QUEUE_PIPE1m,
                        MMU_THDU_OFFSET_QUEUE_PIPE2m,
                        MMU_THDU_OFFSET_QUEUE_PIPE3m,
                        MMU_THDU_CONFIG_QGROUP_PIPE0m,
                        MMU_THDU_CONFIG_QGROUP_PIPE1m,
                        MMU_THDU_CONFIG_QGROUP_PIPE2m,
                        MMU_THDU_CONFIG_QGROUP_PIPE3m,
                        MMU_THDU_OFFSET_QGROUP_PIPE0m,
                        MMU_THDU_OFFSET_QGROUP_PIPE1m,
                        MMU_THDU_OFFSET_QGROUP_PIPE2m,
                        MMU_THDU_OFFSET_QGROUP_PIPE3m,
                        MMU_THDU_CONFIG_PORT_PIPE0m,
                        MMU_THDU_CONFIG_PORT_PIPE1m,
                        MMU_THDU_CONFIG_PORT_PIPE2m,
                        MMU_THDU_CONFIG_PORT_PIPE3m,
                        MMU_THDU_RESUME_PORT_PIPE0m,
                        MMU_THDU_RESUME_PORT_PIPE1m,
                        MMU_THDU_RESUME_PORT_PIPE2m,
                        MMU_THDU_RESUME_PORT_PIPE3m,
                        MMU_WRED_DROP_CURVE_PROFILE_0m,
                        MMU_WRED_DROP_CURVE_PROFILE_1m,
                        MMU_WRED_DROP_CURVE_PROFILE_2m,
                        MMU_WRED_DROP_CURVE_PROFILE_3m,
                        MMU_WRED_DROP_CURVE_PROFILE_4m,
                        MMU_WRED_DROP_CURVE_PROFILE_5m,
                        MMU_WRED_DROP_CURVE_PROFILE_6m,
                        MMU_WRED_DROP_CURVE_PROFILE_7m,
                        MMU_CFAP_BANK0_XPE0m,
                        MMU_CFAP_BANK0_XPE1m,
                        MMU_CFAP_BANK0_XPE2m,
                        MMU_CFAP_BANK0_XPE3m,
                        MMU_CFAP_BANK1_XPE0m,
                        MMU_CFAP_BANK1_XPE1m,
                        MMU_CFAP_BANK1_XPE2m,
                        MMU_CFAP_BANK1_XPE3m,
                        MMU_CFAP_BANK2_XPE0m,
                        MMU_CFAP_BANK2_XPE1m,
                        MMU_CFAP_BANK2_XPE2m,
                        MMU_CFAP_BANK2_XPE3m,
                        MMU_CFAP_BANK3_XPE0m,
                        MMU_CFAP_BANK3_XPE1m,
                        MMU_CFAP_BANK3_XPE2m,
                        MMU_CFAP_BANK3_XPE3m,
                        MMU_CFAP_BANK4_XPE0m,
                        MMU_CFAP_BANK4_XPE1m,
                        MMU_CFAP_BANK4_XPE2m,
                        MMU_CFAP_BANK4_XPE3m,
                        MMU_CFAP_BANK5_XPE0m,
                        MMU_CFAP_BANK5_XPE1m,
                        MMU_CFAP_BANK5_XPE2m,
                        MMU_CFAP_BANK5_XPE3m,
                        MMU_CFAP_BANK6_XPE0m,
                        MMU_CFAP_BANK6_XPE1m,
                        MMU_CFAP_BANK6_XPE2m,
                        MMU_CFAP_BANK6_XPE3m,
                        MMU_CFAP_BANK7_XPE0m,
                        MMU_CFAP_BANK7_XPE1m,
                        MMU_CFAP_BANK7_XPE2m,
                        MMU_CFAP_BANK7_XPE3m,
                        MMU_CFAP_BANK8_XPE0m,
                        MMU_CFAP_BANK8_XPE1m,
                        MMU_CFAP_BANK8_XPE2m,
                        MMU_CFAP_BANK8_XPE3m,
                        MMU_CFAP_BANK9_XPE0m,
                        MMU_CFAP_BANK9_XPE1m,
                        MMU_CFAP_BANK9_XPE2m,
                        MMU_CFAP_BANK9_XPE3m,
                        MMU_CFAP_BANK10_XPE0m,
                        MMU_CFAP_BANK10_XPE1m,
                        MMU_CFAP_BANK10_XPE2m,
                        MMU_CFAP_BANK10_XPE3m,
                        MMU_CFAP_BANK11_XPE0m,
                        MMU_CFAP_BANK11_XPE1m,
                        MMU_CFAP_BANK11_XPE2m,
                        MMU_CFAP_BANK11_XPE3m,
                        MMU_CFAP_BANK12_XPE0m,
                        MMU_CFAP_BANK12_XPE1m,
                        MMU_CFAP_BANK12_XPE2m,
                        MMU_CFAP_BANK12_XPE3m,
                        MMU_CFAP_BANK13_XPE0m,
                        MMU_CFAP_BANK13_XPE1m,
                        MMU_CFAP_BANK13_XPE2m,
                        MMU_CFAP_BANK13_XPE3m,
                        MMU_CFAP_BANK14_XPE0m,
                        MMU_CFAP_BANK14_XPE1m,
                        MMU_CFAP_BANK14_XPE2m,
                        MMU_CFAP_BANK14_XPE3m
};
#else
/* Dummy array to get through preco error for unsupported chips */
static soc_mem_t th_do_not_test_list[1];
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
static soc_mem_t td2p_do_not_test_list[] = {
                        DLB_HGT_FLOWSETm,
                        DLB_HGT_FLOWSET_Xm,
                        DLB_HGT_FLOWSET_Ym,
                        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_Xm,
                        DLB_HGT_FLOWSET_TIMESTAMP_PAGE_Ym,
                        DLB_HGT_GROUP_STATS_Xm,
                        DLB_HGT_GROUP_STATS_Ym,
                        DLB_HGT_OPTIMAL_CANDIDATE_Xm,
                        DLB_HGT_OPTIMAL_CANDIDATE_Ym,
                        EFP_COUNTER_TABLE_Xm,
                        EFP_COUNTER_TABLE_Ym,
                        EFP_METER_TABLE_Xm,
                        EFP_METER_TABLE_Ym,
                        EGR_FLEX_CTR_COUNTER_TABLE_0_Xm,
                        EGR_FLEX_CTR_COUNTER_TABLE_0_Ym,
                        EGR_FLEX_CTR_COUNTER_TABLE_1_Xm,
                        EGR_FLEX_CTR_COUNTER_TABLE_1_Ym,
                        EGR_FLEX_CTR_COUNTER_TABLE_2_Xm,
                        EGR_FLEX_CTR_COUNTER_TABLE_2_Ym,
                        EGR_FLEX_CTR_COUNTER_TABLE_3_Xm,
                        EGR_FLEX_CTR_COUNTER_TABLE_3_Ym,
                        EGR_FRAGMENT_ID_TABLE_Xm,
                        EGR_FRAGMENT_ID_TABLE_Ym,
                        EGR_IP_TUNNEL_IPV6m,
                        EGR_MMU_CREDIT_LIMIT_Xm,
                        EGR_MMU_CREDIT_LIMIT_Ym,
                        EGR_PERQ_XMT_COUNTERS_Xm,
                        EGR_PERQ_XMT_COUNTERS_Ym,
                        EGR_PW_INIT_COUNTERS_Xm,
                        EGR_PW_INIT_COUNTERS_Ym,
                        EGR_SER_FIFOm,
                        FP_COUNTER_TABLE_Xm,
                        FP_COUNTER_TABLE_Ym,
                        FP_GLOBAL_MASK_TCAMm,
                        FP_STORM_CONTROL_METERS_Xm,
                        FP_STORM_CONTROL_METERS_Ym,
                        IARB_MAIN_TDM_Xm,
                        IARB_MAIN_TDM_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_0_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_0_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_1_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_1_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_2_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_2_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_3_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_3_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_4_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_4_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_5_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_5_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_6_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_6_Ym,
                        ING_FLEX_CTR_COUNTER_TABLE_7_Xm,
                        ING_FLEX_CTR_COUNTER_TABLE_7_Ym,
                        ING_PW_TERM_SEQ_NUM_Xm,
                        ING_PW_TERM_SEQ_NUM_Ym,
                        ING_SERVICE_PRI_MAPm,
                        ING_SER_FIFOm,
                        ING_SNAT_HIT_ONLY_Xm,
                        ING_SNAT_HIT_ONLY_Ym,
                        L2_HITDA_ONLY_Xm,
                        L2_HITDA_ONLY_Ym,
                        L2_HITSA_ONLY_Xm,
                        L2_HITSA_ONLY_Ym,
                        L2_ENTRY_LPm,
                        L3_DEFIP_HIT_ONLY_Xm,
                        L3_DEFIP_HIT_ONLY_Ym,
                        L3_DEFIP_PAIR_128_HIT_ONLY_Xm,
                        L3_DEFIP_PAIR_128_HIT_ONLY_Ym,
                        L3_ENTRY_HIT_ONLYm,
                        L3_ENTRY_HIT_ONLY_Xm,
                        L3_ENTRY_HIT_ONLY_Ym,
                        L3_ENTRY_IPV4_MULTICASTm,
                        L3_ENTRY_IPV4_UNICASTm,
                        L3_ENTRY_IPV6_MULTICASTm,
                        L3_ENTRY_IPV6_UNICASTm,
                        L3_ENTRY_LPm,
                        LMEPm,
                        LMEP_DAm,
                        MAID_REDUCTIONm,
                        MA_INDEXm,
                        MA_STATEm,
                        MMU_CFAP_BANK0m,
                        MMU_CFAP_BANK1m,
                        MMU_CFAP_BANK2m,
                        MMU_CFAP_BANK3m,
                        MMU_CFAP_BANK4m,
                        MMU_CFAP_BANK5m,
                        MMU_CFAP_BANK6m,
                        MMU_CFAP_BANK7m,
                        MMU_CFAP_BANK8m,
                        MMU_CFAP_BANK9m,
                        MMU_CFAP_BANK10m,
                        MMU_CFAP_BANK11m,
                        MMU_CFAP_BANK12m,
                        MMU_CFAP_BANK13m,
                        MMU_CFAP_BANK14m,
                        MMU_CFAP_BANK15m,
                        MMU_CFAP_BANK16m,
                        MMU_CFAP_BANK17m,
                        MMU_CFAP_BANK18m,
                        MMU_CFAP_BANK19m,
                        MMU_CFAP_BANK20m,
                        MMU_INTFI_PFC_ST_TBLm,
                        OAM_LM_COUNTERSm,
                        RH_HGT_DROPS_Xm,
                        RH_HGT_DROPS_Ym,
                        RMEPm,
                        SER_MEMORYm,
                        XLPORT_WC_UCMEM_DATAm
};
#else
/* Dummy array to get through preco error for unsupported chips */
static soc_mem_t td2p_do_not_test_list[1];
#endif /* BCM_TRIDENT2_SUPPORT */

/* List of TCAMs to be covered. This list is maintained separately because a lot
   of memories marked as CAMs are actually aggregates having both an SRAM and a
   CAM. This list is generated by searching for names with --A flags from the
   listmem command, duplicates like _[X|Y] and _PIPE[0-3] */
#ifdef BCM_TOMAHAWK_SUPPORT
static soc_mem_t th_tcam_test_list[] = {
                        L3_TUNNEL_ONLYm,
                        FP_UDF_TCAMm,
                        VLAN_SUBNET_ONLYm,
                        VFP_TCAMm,
                        MY_STATION_TCAM_ENTRY_ONLYm,
                        IP_MULTICAST_TCAMm,
                        L2_USER_ENTRY_ONLYm,
                        L3_DEFIP_ONLYm,
                        EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm,
                        DST_COMPRESSION_TCAM_ONLYm,
                        SRC_COMPRESSION_TCAM_ONLYm,
                        IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm,
                        IFP_TCAMm,
                        ING_SNAT_ONLYm,
                        CPU_COS_MAP_ONLYm,
                        EFP_TCAMm
};
#else
/* Dummy array to get through preco error for unsupported chips */
static soc_mem_t th_tcam_test_list[1];
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
static soc_mem_t td2p_tcam_test_list[] = {
                        CPU_COS_MAP_ONLYm,
                        EFP_TCAMm,
                        FP_GM_FIELDSm,
                        FP_TCAMm,
                        FP_UDF_TCAMm,
                        ING_SNAT_ONLYm,
                        IP_MULTICAST_TCAMm,
                        L2_USER_ENTRY_ONLYm,
                        L3_DEFIP_ONLYm,
                        L3_TUNNEL_ONLYm,
                        MY_STATION_TCAM_ENTRY_ONLYm,
                        MY_STATION_TCAM_2_ENTRY_ONLYm,
                        SUBPORT_TAG_SGPP_MAP_ONLYm,
                        UDF_CONDITIONAL_CHECK_TABLE_CAMm,
                        VFP_TCAMm,
                        VLAN_SUBNET_ONLYm
};
#else
/* Dummy array to get through preco error for unsupported chips */
static soc_mem_t td2p_tcam_test_list[1];
#endif /* BCM_TRIDENT2_SUPPORT */

static void
th_get_valid_unique_acc_types(int unit, soc_mem_t mem,
                              uint32 *unique_acc_types)
{
    int block;
    int base_type;
    uint32 memidx;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    memidx = meminfo->base & ser_ih_p->memidx_mask;

    unique_acc_types[0] = ser_ih_p->acc_type_unique[0];
    unique_acc_types[1] = ser_ih_p->acc_type_unique[1];
    unique_acc_types[2] = ser_ih_p->acc_type_unique[2];
    unique_acc_types[3] = ser_ih_p->acc_type_unique[3];

    base_type = SOC_MEM_BASE_TYPE(unit, mem);

    SOC_MEM_BLOCK_ITER(unit, mem, block) {
        if (block == MMU_XPE_BLOCK(unit)) {
            switch (base_type) {
                case TH_BASE_TYPE_IPIPE :
                    if ((memidx / 32768 == 0)
                        || (memidx / 32768 == 3)) {
                        unique_acc_types[2] = INVALID_ACC_TYPE;
                        unique_acc_types[3] = INVALID_ACC_TYPE;
                    } else {
                        unique_acc_types[0] = INVALID_ACC_TYPE;
                        unique_acc_types[1] = INVALID_ACC_TYPE;
                    }
                    break;
                case TH_BASE_TYPE_EPIPE :
                    if ((memidx / 32768 == 0)
                        || (memidx / 32768 == 1)) {
                        unique_acc_types[1] = INVALID_ACC_TYPE;
                        unique_acc_types[3] = INVALID_ACC_TYPE;
                    } else {
                        unique_acc_types[0] = INVALID_ACC_TYPE;
                        unique_acc_types[2] = INVALID_ACC_TYPE;
                    }
                    break;
                case TH_BASE_TYPE_SLICE :
                    if (memidx / 32768 == 0) {
                        unique_acc_types[1] = INVALID_ACC_TYPE;
                        unique_acc_types[3] = INVALID_ACC_TYPE;
                    } else {
                        unique_acc_types[0] = INVALID_ACC_TYPE;
                        unique_acc_types[2] = INVALID_ACC_TYPE;
                    }
                    break;
                case TH_BASE_TYPE_LAYER :
                    if (memidx / 32768 == 0) {
                        unique_acc_types[2] = INVALID_ACC_TYPE;
                        unique_acc_types[3] = INVALID_ACC_TYPE;
                    } else {
                        unique_acc_types[0] = INVALID_ACC_TYPE;
                        unique_acc_types[1] = INVALID_ACC_TYPE;
                    }
                    break;
                default :
                    unique_acc_types[0] = INVALID_ACC_TYPE;
                    unique_acc_types[1] = INVALID_ACC_TYPE;
                    unique_acc_types[2] = INVALID_ACC_TYPE;
                    unique_acc_types[3] = INVALID_ACC_TYPE;
                    break;
            }
        } else if (block == MMU_SC_BLOCK(unit)) {
            switch (base_type) {
                case TH_BASE_TYPE_IPIPE :
                case TH_BASE_TYPE_LAYER:
                    unique_acc_types[2] = INVALID_ACC_TYPE;
                    unique_acc_types[3] = INVALID_ACC_TYPE;
                    break;
                default:
                    unique_acc_types[0] = INVALID_ACC_TYPE;
                    unique_acc_types[1] = INVALID_ACC_TYPE;
                    unique_acc_types[2] = INVALID_ACC_TYPE;
                    unique_acc_types[3] = INVALID_ACC_TYPE;
                    break;
            }
        }
    }
}

static void
get_chip_specific_data(int unit)
{
    uint16 dev_id;
    uint8 rev_id;

    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    soc_cm_get_id (unit, &dev_id, &rev_id);

    cli_out("\nDevice ID = 0x%04x, Rev ID = %0d", dev_id, rev_id);

    if (dev_id == BCM56960_DEVICE_ID || dev_id == BCM56860_DEVICE_ID ||
        dev_id == BCM56970_DEVICE_ID) {
        ser_ih_p->sbus_v4 = 1;
        ser_ih_p->memidx_mask = SBUS_V4_MMU_MEMIDX_MASK;
        ser_ih_p->num_unique_acc_types = TH_NUM_UNIQUE_ACC_TYPES;
        ser_ih_p->num_acc_types = TH_NUM_VALID_ACC_TYPES;
        ser_ih_p->acc_type_duplicate = TH_ACC_TYPE_DUPLICATE;
        ser_ih_p->acc_type_single = TH_ACC_TYPE_SINGLE;
        ser_ih_p->acc_type_data_split = TH_ACC_TYPE_DATA_SPLIT;
        ser_ih_p->acc_type_addr_split_dist = TH_ACC_TYPE_ADDR_SPLIT_DIST;
        ser_ih_p->acc_type_addr_split_split = TH_ACC_TYPE_ADDR_SPLIT_SPLIT;

        ser_ih_p->acc_type_unique = (uint32*)
                                    sal_alloc(ser_ih_p->num_unique_acc_types *
                                                        sizeof(uint32),
                                                       "acc_type_unique");
        ser_ih_p->acc_type_unique[0] = TH_ACC_TYPE_UNIQUE_PIPE0;
        ser_ih_p->acc_type_unique[1] = TH_ACC_TYPE_UNIQUE_PIPE1;
        ser_ih_p->acc_type_unique[2] = TH_ACC_TYPE_UNIQUE_PIPE2;
        ser_ih_p->acc_type_unique[3] = TH_ACC_TYPE_UNIQUE_PIPE3;

        ser_ih_p->memidx_mask = SBUS_V4_MMU_MEMIDX_MASK;
        ser_ih_p->get_valid_unique_acc_types = &th_get_valid_unique_acc_types;

    } else {
        cli_out("\nUnsupported chip");
        ser_ih_p->bad_input = 1;
    }
}

static int
is_acc_type_unique(int unit, soc_mem_t mem)
{
    int rv = 0;
    int i;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    for (i = 0; i < ser_ih_p->num_unique_acc_types; i++) {
        if (SOC_MEM_ACC_TYPE(unit, mem) == ser_ih_p->acc_type_unique[i]) {
            rv = 1;
        }
    }

    return rv;
}


static int
do_not_test(int unit, soc_mem_t mem)
{
    int i;
    int rv = 0;
    int num_do_not_test = 0;
    soc_mem_t dummy_list[] = {0};
    soc_mem_t *do_not_test_list;

    if (SOC_IS_TOMAHAWKX(unit)) {
        num_do_not_test = sizeof(th_do_not_test_list) /
                          sizeof(th_do_not_test_list[0]);
        do_not_test_list = th_do_not_test_list;
    } else if (SOC_IS_TRIDENT2PLUS(unit)) {
        num_do_not_test = sizeof(td2p_do_not_test_list) /
                          sizeof(td2p_do_not_test_list[0]);
        do_not_test_list = td2p_do_not_test_list;
    } else {
        do_not_test_list = dummy_list;
    }
    for (i = 0; i < num_do_not_test; i++) {
        if (mem == do_not_test_list[i]) {
            rv = 1;
            return rv;
        }
    }

    if (SOC_IS_TOMAHAWK2(unit)) {
        num_do_not_test = sizeof(th2_do_not_test_list) /
                          sizeof(th2_do_not_test_list[0]);
        do_not_test_list = th2_do_not_test_list;

        for (i = 0; i < num_do_not_test; i++) {
            if (mem == do_not_test_list[i]) {
                rv = 1;
                return rv;
            }
        }
    }

    return rv;
}

static int
test_tcam(int unit, soc_mem_t mem) {
    int i;
    int rv = 0;
    int num_tcam_test = 0;
    soc_mem_t dummy_list[] = {0};
    soc_mem_t *tcam_test_list;

    if (SOC_IS_TOMAHAWKX(unit)) {
        num_tcam_test = sizeof(th_tcam_test_list) /
                        sizeof(th_tcam_test_list[0]);
        tcam_test_list = th_tcam_test_list;
    } else if (SOC_IS_TRIDENT2PLUS(unit)) {
        num_tcam_test = sizeof(td2p_tcam_test_list) /
                        sizeof(td2p_tcam_test_list[0]);
        tcam_test_list = td2p_tcam_test_list;
    } else {
        tcam_test_list = dummy_list;
    }
    for (i = 0; i < num_tcam_test; i++) {
        if (mem == tcam_test_list[i]) {
            rv = 1;
        }
    }
    return rv;
}

static int
test_mem(int unit, soc_mem_t mem)
{
    int rv = 0;
    uint32 num_entries;
    uint32 flags = 0;

    if (SOC_MEM_IS_VALID(unit, mem)) {
        num_entries = soc_mem_index_max(unit, mem)
                                        - soc_mem_index_min(unit, mem) + 1;
        flags = SOC_MEM_INFO(unit, mem).flags;
        if ((!((flags & SOC_MEM_FLAG_AGGR) || (flags & SOC_MEM_FLAG_READONLY)))
                && (num_entries >= 2) && (do_not_test(unit, mem) == 0)) {
            if (flags & SOC_MEM_FLAG_CAM) {
                if (test_tcam(unit, mem) == 1) {
                    rv = 1;
                }
            } else {
                rv = 1;
            }
        }
    }
    return rv;
}

static int
duplicate_mem(int unit, soc_mem_t mem) {
    int i;
    int rv = 0;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);

    if ((test_mem(unit, mem) == 1)
                && (is_acc_type_unique(unit, mem) == 1)) {
        for (i = 0; i < NUM_SOC_MEM; i++) {
            if (test_mem(unit, i) == 1) {
                if (meminfo->base == SOC_MEM_INFO(unit, i).base) {
                    if ((SOC_MEM_ACC_TYPE(unit, i)
                                    == ser_ih_p->acc_type_duplicate)
                        || (SOC_MEM_ACC_TYPE(unit, i)
                                            == ser_ih_p->acc_type_data_split)
                        ) {
                            rv = 1;
                    }
                }
            }
        }
    }
    return rv;
}



static int
do_unique_access(int unit, soc_mem_t mem)
{
    int rv = 0;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    if (SOC_MEM_ACC_TYPE(unit, mem) == ser_ih_p->acc_type_duplicate) {
        rv = 1;
    }

    return rv;
}

static void
ser_ih_parse_test_params(int unit, args_t *a)
{
    parse_table_t parse_table;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    const char tr901_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "This test is intended to be used for in house SER testing. The test can be\n"
    "to write all SW accessible memories on chip and read them back to check for\n"
    "flips. The assumption is that this test will be run at regular intervals in a\n"
    "loop that is part of a master soc script as per test requirements. The test\n"
    "provides information on the total memory tested, total number of flips and\n"
    "the logical coordinate of each flip. However it does not provide information\n"
    "about the physical coordinate of each flip.\n"
    "\n"
    "Configuration parameters passed from CLI:\n"
    "TestPat: Test Pattern used to fill the memories\n"
    "        0: All 0s\n"
    "        1: All 1s\n"
    "        2: Logical checker board\n"
    "        3: Logical inverse checker board\n"
    "RdWrMode: Whether the test writes or reads the memories\n"
    "        1: Test only writes all the memories under test\n"
    "        2: Test only reads back all memories under test and counts flips\n";
#endif

    ser_ih_p->bad_input = 0;

    ser_ih_p->test_pattern_param = TEST_PATTERN_PARAM_DEFAULT;
    ser_ih_p->rd_wr_mode_param = RD_WR_MODE_PARAM_DEFAULT;
    ser_ih_p->quiet_mode_param = QUIET_MODE_PARAM_DEFAULT;
    /*Parse CLI opts */

    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "TestPat", PQ_INT | PQ_DFL,
                    0, &(ser_ih_p->test_pattern_param), NULL);
    parse_table_add(&parse_table, "RdWrMode", PQ_INT | PQ_DFL,
                    0, &(ser_ih_p->rd_wr_mode_param), NULL);
    parse_table_add(&parse_table, "QuietMode", PQ_INT | PQ_DFL,
                    0, &(ser_ih_p->quiet_mode_param), NULL);


    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr901_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        ser_ih_p->bad_input = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("\n-----------------");
        cli_out("\n TEST PARAMETERS ");
        cli_out("\n-----------------");
        cli_out("\ntest_pattern = %0d", ser_ih_p->test_pattern_param);
        cli_out("\nrd_wr_mode   = %0d", ser_ih_p->rd_wr_mode_param);
        cli_out("\nquiet_mode   = %0d", ser_ih_p->quiet_mode_param);
        cli_out("\n-----------------");
    }
}

static int
num_bits_mask(int unit, soc_mem_t mem)
{
    int i, j;
    uint32 entry_words;
    uint32 num_bits = 0;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    entry_words = soc_mem_entry_words(unit, mem);

    for (i = 0; i < entry_words; i++) {
        for (j = 0; j < MEM_WORD; j++) {
            if (((0x00000001 << j) & ser_ih_p->mask_array[mem][i]) != 0x0) {
                num_bits++;
            }
        }
    }

    return num_bits;
}

static void
write_mem(int unit, soc_mem_t mem, uint32 test_pattern)
{
    uint32 i, j;
    uint32 *wr_data;
    uint32 base_word;
    uint32 wr_word;
    uint32 entry_words;
    uint32 flags = SOC_MEM_DONT_USE_CACHE | SOC_MEM_DONT_CONVERT_XY2DM;
    uint32 meminfo_flags;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);

    meminfo_flags = meminfo->flags;
    meminfo->flags &= (~SOC_MEM_FLAG_CAM);

    switch (test_pattern) {
        case TEST_PATTERN_ALL0 :
            base_word = 0x00000000;
            break;
        case TEST_PATTERN_ALL1 :
            base_word = 0xffffffff;
            break;
        case TEST_PATTERN_CHB :
            base_word = CHB_FIRST_WORD;
            break;
        case TEST_PATTERN_ICHB :
            base_word = ICHB_FIRST_WORD;
            break;
        default:
            base_word = 0x00000000;
            break;
    }

    for (i = soc_mem_index_min(unit, mem);
         i < (soc_mem_index_max(unit, mem) + 1);
         i++) {

        wr_word = base_word;
        entry_words = soc_mem_entry_words(unit, mem);
        wr_data = (uint32*)sal_alloc(entry_words * sizeof(uint32), "wr_data");

        for (j = 0; j < entry_words; j++) {
            wr_data[j] = wr_word;
        }

        (void)soc_mem_write_extended(unit, flags, mem, COPYNO_ALL, i, wr_data);

        if (test_pattern == TEST_PATTERN_CHB
                || test_pattern == TEST_PATTERN_ICHB) {
            base_word = ~base_word;
        }

        sal_free(wr_data);
    }

    meminfo->flags = meminfo_flags;
}

static void
write_all_mems(int unit)
{
    int i;
    soc_mem_t mem;
    uint32 acc_type;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    if (ser_ih_p->quiet_mode_param) {
        cli_out("\nWriting all memories...");
    }
    for (i = 0; i < NUM_SOC_MEM; i++) {
        mem = (soc_mem_t)(i);

        if (test_mem(unit, mem) == 1 && duplicate_mem(unit, mem) == 0) {
            acc_type = SOC_MEM_ACC_TYPE(unit, mem);
            if (!ser_ih_p->quiet_mode_param) {
                cli_out("\nWriting %s, acc_type = %0d, base = 0x%08x",
                        SOC_MEM_NAME(unit, mem), acc_type,
                        SOC_MEM_INFO(unit, mem).base);
            }
            write_mem(unit, mem, ser_ih_p->test_pattern_param);
            ser_ih_p->total_memories_written++;
        }
    }
}

static void
read_and_check_mem(int unit, soc_mem_t mem, uint32 test_pattern)
{
    uint32 i, j, k;
    uint32 mem_word_bit;
    uint32 *rd_data;
    uint32 base_word;
    uint32 exp_word;
    uint32 entry_words;
    uint32 flags = SOC_MEM_DONT_USE_CACHE | SOC_MEM_DONT_CONVERT_XY2DM;
    uint32 use_unique_access_types = 0;
    uint32 mask;
    uint32 fail_row;
    uint32 fail_col;
    uint32 fail_cnt;
    uint32 num_entries;
    uint32 meminfo_flags;
    uint32 comp_mask;
    uint32 *valid_acc_types;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);

    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    meminfo_flags = meminfo->flags;

    meminfo_flags = meminfo->flags;
    meminfo->flags &= (~SOC_MEM_FLAG_CAM);

    use_unique_access_types = do_unique_access(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);
    num_entries = soc_mem_index_max(unit, mem)
                                - soc_mem_index_min(unit, mem) + 1;

    valid_acc_types = (uint32*) sal_alloc(ser_ih_p->num_unique_acc_types *
                                            sizeof(uint32), "acc_type_unique");

    ser_ih_p->get_valid_unique_acc_types(unit, mem, valid_acc_types);

    for (i = 0; i < ser_ih_p->num_unique_acc_types; i++) {

        switch (test_pattern) {
            case TEST_PATTERN_ALL0 :
                base_word = 0x00000000;
                break;
            case TEST_PATTERN_ALL1 :
                base_word = 0xffffffff;
                break;
            case TEST_PATTERN_CHB :
                base_word = CHB_FIRST_WORD;
                break;
            case TEST_PATTERN_ICHB :
                base_word = ICHB_FIRST_WORD;
                break;
            default:
                base_word = 0x00000000;
                break;
        }
        if (use_unique_access_types == 0 && i > 0) {
            break;
        } else if (use_unique_access_types == 0 && i == 0) {
            if (!ser_ih_p->quiet_mode_param) {
                cli_out("\nReading and Checking %s", SOC_MEM_NAME(unit, mem));
            }
            ser_ih_p->total_memories_read++;
            ser_ih_p->total_bits_tested
                                += num_entries * num_bits_mask(unit, mem);
        } else {
            if (valid_acc_types[i] != INVALID_ACC_TYPE) {
                if (!ser_ih_p->quiet_mode_param) {
                    cli_out("\nReading and Checking %s for acc_type %0d",
                            SOC_MEM_NAME(unit, mem), valid_acc_types[i]);
                }
                ser_ih_p->total_memories_read++;
                ser_ih_p->total_bits_tested
                                    += num_entries * num_bits_mask(unit, mem);
            } else {
                continue;
            }
        }

        fail_cnt = 0;
        for (j = soc_mem_index_min(unit, mem);
            j < (soc_mem_index_max(unit, mem) + 1);
            j++) {
            exp_word = base_word;
            rd_data = (uint32*)sal_alloc(entry_words * sizeof(uint32),
                                         "rd_data");
            if (use_unique_access_types == 0) {
                soc_mem_read_extended(unit, flags, mem, 0,
                                        COPYNO_ALL, j, rd_data);
            } else {
                soc_mem_pipe_select_read(unit, flags, mem, COPYNO_ALL,
                                         valid_acc_types[i], j, rd_data);
            }

            for (k = 0; k < entry_words; k++) {
                mask = ser_ih_p->mask_array[mem][k];

                if (rd_data[k] !=
                        ((exp_word & mask) | (rd_data[k] & (~mask)))) {
                    if (!ser_ih_p->quiet_mode_param) {
                        cli_out("\nexp_word = 0x%08x, rd_data = 0x%08x",
                                exp_word, rd_data[k]);
                    }
                    for (mem_word_bit = 0;
                        mem_word_bit < MEM_WORD;
                        mem_word_bit++) {
                        comp_mask = (0x00000001 << mem_word_bit) & mask;
                        if ((rd_data[k] & comp_mask)
                                != (exp_word & comp_mask)) {
                            if (!ser_ih_p->quiet_mode_param) {
                                cli_out("\nmask = 0x%08x", mask);
                            }
                            fail_row = j;
                            fail_col = (k * MEM_WORD) + mem_word_bit;
                            fail_cnt++;
                            if (!ser_ih_p->quiet_mode_param) {
                                cli_out("\nFlip: Row = %0d, Col = %0d",
                                        fail_row, fail_col);
                            }
                            if (fail_cnt >= MAX_FAIL_COUNT) {
                                test_error(unit,"\n*ERROR: Max flip count of %0d "
                                                "exceeded for %s acc_type %0d",
                                            MAX_FAIL_COUNT,
                                            SOC_MEM_NAME(unit, mem),
                                            valid_acc_types[i]);
                                ser_ih_p->test_fail = 1;
                                break;
                            }
                        }
                    }
                }
            }

            if (test_pattern == TEST_PATTERN_CHB
                    || test_pattern == TEST_PATTERN_ICHB) {
                base_word = ~base_word;
            }

            sal_free(rd_data);
        }
        if (ser_ih_p->quiet_mode_param && fail_cnt > 0) {
            cli_out("\n*ERROR: %s acc_type %0d had %0d bit flipped",
                    SOC_MEM_NAME(unit, mem), valid_acc_types[i], fail_cnt);
        }
        ser_ih_p->total_flips += fail_cnt;
    }

    sal_free(valid_acc_types);
    meminfo->flags = meminfo_flags;
}

static void
read_and_check_all_mems(int unit)
{
    int i;
    soc_mem_t mem;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    if (ser_ih_p->quiet_mode_param) {
        cli_out("\nReading and Checking all memories...");
    }
    for (i = 0; i < NUM_SOC_MEM; i++) {
        mem = (soc_mem_t)(i);

        if (test_mem(unit, mem) == 1 && duplicate_mem(unit, mem) == 0) {
            read_and_check_mem(unit, mem, ser_ih_p->test_pattern_param);
        }
    }
}

int
ser_ih_test_init(int unit, args_t *a, void **pa)
{
    int i, j;
    soc_mem_t mem;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    SOC_MEM_FORCE_READ_THROUGH_SET(unit, 1);

    ser_ih_p = sal_alloc(sizeof(ser_ih_t), "ser_ih_test");
    sal_memset(ser_ih_p, 0, sizeof(ser_ih_t));
    ser_ih_parray[unit] = ser_ih_p;

    ser_ih_parse_test_params(unit, a);

    ser_ih_p->mask_array = (uint32**)sal_alloc(NUM_SOC_MEM * sizeof(uint32*),
                                         "mask_array*");

    for (i = 0; i < NUM_SOC_MEM; i++) {
        ser_ih_p->mask_array[i] = (uint32*)sal_alloc(SOC_MAX_MEM_WORDS *
                                                                sizeof(uint32),
                                                        "mask_array");
    }

    for (i = 0; i < NUM_SOC_MEM; i++) {
        for (j = 0; j < SOC_MAX_MEM_WORDS; j++) {
            ser_ih_p->mask_array[i][j] = 0xffffffff;
        }
    }

    for (i = 0; i < NUM_SOC_MEM; i++) {
        mem = (soc_mem_t)(i);

        if (SOC_MEM_IS_VALID(unit, mem)) {
            soc_mem_datamask_get(unit, mem, ser_ih_p->mask_array[i]);
        }
    }

    ser_ih_p->total_memories_written = 0;
    ser_ih_p->total_memories_read = 0;
    ser_ih_p->total_flips = 0;
    ser_ih_p->total_bits_tested = 0;

    get_chip_specific_data(unit);

    if (ser_ih_p->bad_input == 1) {
        return BCM_E_FAIL;
    } else {
        return BCM_E_NONE;
    }
}

int
ser_ih_test(int unit, args_t *a, void *pa)
{
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    if (ser_ih_p->rd_wr_mode_param == WRITE_ONLY) {
        write_all_mems(unit);
    } else if (ser_ih_p->rd_wr_mode_param == READ_ONLY) {
        read_and_check_all_mems(unit);
    }
    return 0;
}

int
ser_ih_test_cleanup(int unit, void *pa)
{
    int i;
    ser_ih_t *ser_ih_p = ser_ih_parray[unit];

    cli_out("\n-----------------");
    cli_out("\n   TEST RESULT   ");
    cli_out("\n-----------------");
    if (ser_ih_p->rd_wr_mode_param == WRITE_ONLY) {
        cli_out("\nTotal memories written = %0d", ser_ih_p->total_memories_written);
    } else if (ser_ih_p->rd_wr_mode_param == READ_ONLY) {
        cli_out("\nTotal memories read = %0d", ser_ih_p->total_memories_read);
        cli_out("\nTotal Flips = %0d", ser_ih_p->total_flips);
        cli_out("\nTotal bits tested = %0d kb", ser_ih_p->total_bits_tested / 1024);
    }
    cli_out("\n-----------------");

    for (i = 0; i < NUM_SOC_MEM; i++) {
        sal_free(ser_ih_p->mask_array[i]);
    }

    sal_free(ser_ih_p->mask_array);

    sal_free(ser_ih_p->acc_type_unique);
    sal_free(ser_ih_p);
    cli_out("\n");
    return 0;
}

#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2_SUPPORT */


