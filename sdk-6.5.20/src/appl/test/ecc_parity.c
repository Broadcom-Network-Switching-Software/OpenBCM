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

#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/mem.h>
#include <soc/field.h>
#include <soc/chip.h>
#include "testlist.h"

extern int bcm_common_linkscan_enable_set(int, int);

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)

#define BIT_IN_UINT32 (sizeof(uint32)*8)

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

/*
 * Macro used by memory accessor functions to fix order
 */
#define FIX_MEM_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BYTES2WORDS((m)->bytes)-1-(v) : \
                                (v))
/*
 * Macro to generate a pseudo random number
 */
#define RNUM(x) (1664525L * (x) + 1013904223L)

typedef void (*get_valid_unique_acc_types_t) (int, soc_mem_t, uint32*);

typedef struct ecc_par_s {
    char   *mem_str;
    soc_mem_t mem;
    uint32 single_entry;
    _soc_ser_test_t test_type;
    uint32 total_memories_tested;
    uint32 total_entries_tested;
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
    uint32 was_debug_mode;
} ecc_par_t;

static ecc_par_t *ecc_par_parray[SOC_MAX_NUM_DEVICES];

/* The do_not_test_list array contains a list of memories not to be tested. This
   includes memories that are updated continuously by hardware even this test is
   ran immediately after "init soc". The 'rc.soc" is not executed as it starts
   more software threads and keeps writing to more memories. The list also
   includes overlays and aggregates that are missed in the soc_mem_info_t. */
#ifdef BCM_TOMAHAWK2_SUPPORT
static soc_mem_t th2_do_not_test_list[] = {
                        SER_RESULT_EXPECTED_0m,
                        FPEM_ECCm,
                        REPLICATION_FIFO_BANK0m,
                        REPLICATION_FIFO_BANK0_XPE0m,
                        REPLICATION_FIFO_BANK0_XPE1m,
                        REPLICATION_FIFO_BANK0_XPE2m,
                        REPLICATION_FIFO_BANK0_XPE3m,
                        REPLICATION_FIFO_BANK1m,
                        REPLICATION_FIFO_BANK1_XPE0m,
                        REPLICATION_FIFO_BANK1_XPE1m,
                        REPLICATION_FIFO_BANK1_XPE2m,
                        REPLICATION_FIFO_BANK1_XPE3m,
                        MMU_INTFO_TC2PRI_MAPPINGm,
                        EGR_IP_TUNNEL_MPLSm,
                        EGR_IP_TUNNEL_IPV6m,
                        MODPORT_MAP_M1m,
                        MODPORT_MAP_M2m,
                        MODPORT_MAP_M3m,
                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm,
                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE0m,
                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE1m,
                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE2m,
                        IFP_LOGICAL_TABLE_SELECT_DATA_ONLY_PIPE3m,
                        TCB_THRESHOLD_PROFILE_MAP_XPE0m,
                        TCB_THRESHOLD_PROFILE_MAP_XPE1m,
                        TCB_THRESHOLD_PROFILE_MAP_XPE2m,
                        TCB_THRESHOLD_PROFILE_MAP_XPE3m
};
#else
/* Dummy array to get through preco error for unsupported chips */
static soc_mem_t th2_do_not_test_list[1];
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
static soc_mem_t th_do_not_test_list[] = {
                        EGR_VLAN_XLATE_ECCm,
                        L2_ENTRY_ONLY_ECCm,
                        L3_ENTRY_ONLY_ECCm,
                        MPLS_ENTRY_ECCm,
                        VLAN_XLATE_ECCm,
                        EGR_1588_SAm,
                        EGR_MAP_MHm,
                        EGR_PORTm,
                        EGR_PRI_CNG_MAPm,
                        EGR_VLANm,
                        EGR_VLAN_STGm,
                        ICONTROL_OPCODE_BITMAPm,
                        IFP_STORM_CONTROL_METERSm,
                        ING_SNAT_DATA_ONLYm,
                        IS_TDM_CALENDAR0m,
                        IS_TDM_CALENDAR1m,
                        L2_MOD_FIFOm,
                        L3_DEFIP_DATA_ONLYm,
                        L3_TUNNEL_DATA_ONLYm,
                        PORT_LAG_FAILOVER_SETm,
                        SER_RESULT_EXPECTED_1m,
                        SFLOW_ING_DATA_SOURCEm,
                        SFLOW_ING_FLEX_DATA_SOURCEm,
                        SRC_MODID_INGRESS_BLOCKm,
                        UNKNOWN_HGI_BITMAPm,
                        VLAN_TABm,
                        CENTRAL_CTR_EVICTION_FIFOm,
                        L3_DEFIP_PAIR_128_DATA_ONLYm,
                        MMU_CCP_MEM_XPE0m,
                        MMU_CCP_MEM_XPE1m,
                        MMU_CCP_MEM_XPE2m,
                        MMU_CCP_MEM_XPE3m,
                        MMU_CCP_RESEQ_MEM_XPE0_PIPE1m,
                        MMU_CCP_RESEQ_MEM_XPE1_PIPE2m,
                        MMU_CCP_RESEQ_MEM_XPE1_PIPE3m,
                        MMU_CCP_RESEQ_MEM_XPE2_PIPE1m,
                        MMU_CCP_RESEQ_MEM_XPE3_PIPE2m,
                        MMU_CCP_RESEQ_MEM_XPE3_PIPE3m,
                        MMU_CELL_LINK_XPE0m,
                        MMU_CELL_LINK_XPE1m,
                        MMU_CELL_LINK_XPE2m,
                        MMU_CELL_LINK_XPE3m,
                        MMU_CFAP_BANK0_XPE0m,
                        MMU_CFAP_BANK0_XPE1m,
                        MMU_CFAP_BANK0_XPE2m,
                        MMU_CFAP_BANK0_XPE3m,
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
                        MMU_CFAP_BANK14_XPE3m,
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
                        MMU_CTR_COLOR_DROP_MEM_XPE0m,
                        MMU_CTR_COLOR_DROP_MEM_XPE1m,
                        MMU_CTR_COLOR_DROP_MEM_XPE2m,
                        MMU_CTR_COLOR_DROP_MEM_XPE3m,
                        MMU_CTR_ING_DROP_MEM_XPE0_PIPE0m,
                        MMU_CTR_ING_DROP_MEM_XPE0_PIPE3m,
                        MMU_CTR_ING_DROP_MEM_XPE1_PIPE0m,
                        MMU_CTR_ING_DROP_MEM_XPE1_PIPE3m,
                        MMU_CTR_ING_DROP_MEM_XPE2_PIPE1m,
                        MMU_CTR_ING_DROP_MEM_XPE2_PIPE2m,
                        MMU_CTR_ING_DROP_MEM_XPE3_PIPE1m,
                        MMU_CTR_ING_DROP_MEM_XPE3_PIPE2m,
                        MMU_CTR_MC_DROP_MEM_XPE0_PIPE1m,
                        MMU_CTR_MC_DROP_MEM_XPE1_PIPE2m,
                        MMU_CTR_MC_DROP_MEM_XPE1_PIPE3m,
                        MMU_CTR_MC_DROP_MEM_XPE2_PIPE1m,
                        MMU_CTR_MC_DROP_MEM_XPE3_PIPE2m,
                        MMU_CTR_MC_DROP_MEM_XPE3_PIPE3m,
                        MMU_CTR_UC_DROP_MEM_XPE0_PIPE1m,
                        MMU_CTR_UC_DROP_MEM_XPE1_PIPE2m,
                        MMU_CTR_UC_DROP_MEM_XPE1_PIPE3m,
                        MMU_CTR_UC_DROP_MEM_XPE2_PIPE1m,
                        MMU_CTR_UC_DROP_MEM_XPE3_PIPE2m,
                        MMU_CTR_UC_DROP_MEM_XPE3_PIPE3m,
                        MMU_CTR_WRED_DROP_MEM_XPE0_PIPE1m,
                        MMU_CTR_WRED_DROP_MEM_XPE1_PIPE2m,
                        MMU_CTR_WRED_DROP_MEM_XPE1_PIPE3m,
                        MMU_CTR_WRED_DROP_MEM_XPE2_PIPE1m,
                        MMU_CTR_WRED_DROP_MEM_XPE3_PIPE2m,
                        MMU_CTR_WRED_DROP_MEM_XPE3_PIPE3m,
                        MMU_ENQS_CT_FIFO_PIPE0m,
                        MMU_ENQS_CT_FIFO_PIPE1m,
                        MMU_ENQS_CT_FIFO_PIPE2m,
                        MMU_ENQS_CT_FIFO_PIPE3m,
                        MMU_ENQS_PBI_DB_SC0_PIPE1m,
                        MMU_ENQS_PBI_DB_SC0_PIPE2m,
                        MMU_ENQS_PBI_DB_SC0_PIPE3m,
                        MMU_ENQS_PBI_DB_SC1_PIPE1m,
                        MMU_ENQS_PBI_DB_SC1_PIPE2m,
                        MMU_ENQS_PBI_DB_SC1_PIPE3m,
                        MMU_ENQX_PIPEMEM_HI_XPE0_PIPE3m,
                        MMU_ENQX_PIPEMEM_HI_XPE1_PIPE3m,
                        MMU_ENQX_PIPEMEM_HI_XPE2_PIPE1m,
                        MMU_ENQX_PIPEMEM_HI_XPE2_PIPE2m,
                        MMU_ENQX_PIPEMEM_HI_XPE3_PIPE1m,
                        MMU_ENQX_PIPEMEM_HI_XPE3_PIPE2m,
                        MMU_ENQX_PIPEMEM_LO_XPE0_PIPE3m,
                        MMU_ENQX_PIPEMEM_LO_XPE1_PIPE3m,
                        MMU_ENQX_PIPEMEM_LO_XPE2_PIPE1m,
                        MMU_ENQX_PIPEMEM_LO_XPE2_PIPE2m,
                        MMU_ENQX_PIPEMEM_LO_XPE3_PIPE1m,
                        MMU_ENQX_PIPEMEM_LO_XPE3_PIPE2m,
                        MMU_MCFP_XPE0m,
                        MMU_MCFP_XPE1m,
                        MMU_MCFP_XPE2m,
                        MMU_MCFP_XPE3m,
                        MMU_MCQDB_X_A_XPE0m,
                        MMU_MCQDB_X_A_XPE1m,
                        MMU_MCQDB_X_A_XPE2m,
                        MMU_MCQDB_X_A_XPE3m,
                        MMU_MCQDB_X_B_XPE0m,
                        MMU_MCQDB_X_B_XPE1m,
                        MMU_MCQDB_X_B_XPE2m,
                        MMU_MCQDB_X_B_XPE3m,
                        MMU_MCQDB_Y_A_XPE0m,
                        MMU_MCQDB_Y_A_XPE1m,
                        MMU_MCQDB_Y_A_XPE2m,
                        MMU_MCQDB_Y_A_XPE3m,
                        MMU_MCQDB_Y_B_XPE0m,
                        MMU_MCQDB_Y_B_XPE1m,
                        MMU_MCQDB_Y_B_XPE2m,
                        MMU_MCQDB_Y_B_XPE3m,
                        MMU_MCQE_XPE0m,
                        MMU_MCQE_XPE1m,
                        MMU_MCQE_XPE2m,
                        MMU_MCQE_XPE3m,
                        MMU_MCQN_XPE0m,
                        MMU_MCQN_XPE1m,
                        MMU_MCQN_XPE2m,
                        MMU_MCQN_XPE3m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE0m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE1m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE2m,
                        MMU_MTRO_BUCKET_L0_MEM_PIPE3m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE0m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE1m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE2m,
                        MMU_MTRO_BUCKET_L1_MEM_PIPE3m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE0m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE1m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE2m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE3m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE0m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE1m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE2m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE3m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE2m,
                        MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE3m,
                        MMU_MTRO_L0_MEM_PIPE0m,
                        MMU_MTRO_L0_MEM_PIPE1m,
                        MMU_MTRO_L0_MEM_PIPE2m,
                        MMU_MTRO_L0_MEM_PIPE3m,
                        MMU_MTRO_L1_MEM_PIPE0m,
                        MMU_MTRO_L1_MEM_PIPE1m,
                        MMU_MTRO_L1_MEM_PIPE2m,
                        MMU_MTRO_L1_MEM_PIPE3m,
                        MMU_PDB_X_XPE0m,
                        MMU_PDB_X_XPE1m,
                        MMU_PDB_X_XPE2m,
                        MMU_PDB_X_XPE3m,
                        MMU_PDB_Y_XPE0m,
                        MMU_PDB_Y_XPE1m,
                        MMU_PDB_Y_XPE2m,
                        MMU_PDB_Y_XPE3m,
                        MMU_PKTHDR_XPE0m,
                        MMU_PKTHDR_XPE1m,
                        MMU_PKTHDR_XPE2m,
                        MMU_PKTHDR_XPE3m,
                        MMU_PKT_LINK_XPE0m,
                        MMU_PKT_LINK_XPE1m,
                        MMU_PKT_LINK_XPE2m,
                        MMU_PKT_LINK_XPE3m,
                        MMU_PORTCNT_XPE0m,
                        MMU_PORTCNT_XPE1m,
                        MMU_PORTCNT_XPE2m,
                        MMU_PORTCNT_XPE3m,
                        MMU_REPL_GROUP_INFO_TBL_PIPE0m,
                        MMU_REPL_GROUP_INFO_TBL_PIPE1m,
                        MMU_REPL_GROUP_INFO_TBL_PIPE2m,
                        MMU_REPL_GROUP_INFO_TBL_PIPE3m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m,
                        MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m,
                        MMU_REPL_HEAD_TBL_PIPE0m,
                        MMU_REPL_HEAD_TBL_PIPE1m,
                        MMU_REPL_HEAD_TBL_PIPE2m,
                        MMU_REPL_HEAD_TBL_PIPE3m,
                        MMU_REPL_LIST_TBL_PIPE0m,
                        MMU_REPL_LIST_TBL_PIPE1m,
                        MMU_REPL_LIST_TBL_PIPE2m,
                        MMU_REPL_LIST_TBL_PIPE3m,
                        MMU_REPL_STATE_TBL_PIPE0m,
                        MMU_REPL_STATE_TBL_PIPE1m,
                        MMU_REPL_STATE_TBL_PIPE2m,
                        MMU_REPL_STATE_TBL_PIPE3m,
                        MMU_THDM_DB_PORTSP_BST_XPE0_PIPE1m,
                        MMU_THDM_DB_PORTSP_BST_XPE1_PIPE2m,
                        MMU_THDM_DB_PORTSP_BST_XPE1_PIPE3m,
                        MMU_THDM_DB_PORTSP_BST_XPE2_PIPE1m,
                        MMU_THDM_DB_PORTSP_BST_XPE3_PIPE2m,
                        MMU_THDM_DB_PORTSP_BST_XPE3_PIPE3m,
                        MMU_THDM_DB_PORTSP_CONFIG_A_PIPE0m,
                        MMU_THDM_DB_PORTSP_CONFIG_A_PIPE1m,
                        MMU_THDM_DB_PORTSP_CONFIG_A_PIPE2m,
                        MMU_THDM_DB_PORTSP_CONFIG_A_PIPE3m,
                        MMU_THDM_DB_PORTSP_CONFIG_B_PIPE0m,
                        MMU_THDM_DB_PORTSP_CONFIG_B_PIPE1m,
                        MMU_THDM_DB_PORTSP_CONFIG_B_PIPE2m,
                        MMU_THDM_DB_PORTSP_CONFIG_B_PIPE3m,
                        MMU_THDM_DB_PORTSP_CONFIG_C_PIPE0m,
                        MMU_THDM_DB_PORTSP_CONFIG_C_PIPE1m,
                        MMU_THDM_DB_PORTSP_CONFIG_C_PIPE2m,
                        MMU_THDM_DB_PORTSP_CONFIG_C_PIPE3m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE0m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE1m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE2m,
                        MMU_THDM_DB_PORTSP_CONFIG_PIPE3m,
                        MMU_THDM_DB_QUEUE_BST_XPE0_PIPE1m,
                        MMU_THDM_DB_QUEUE_BST_XPE1_PIPE2m,
                        MMU_THDM_DB_QUEUE_BST_XPE1_PIPE3m,
                        MMU_THDM_DB_QUEUE_BST_XPE2_PIPE1m,
                        MMU_THDM_DB_QUEUE_BST_XPE3_PIPE2m,
                        MMU_THDM_DB_QUEUE_BST_XPE3_PIPE3m,
                        MMU_THDM_DB_QUEUE_CONFIG_A_PIPE0m,
                        MMU_THDM_DB_QUEUE_CONFIG_A_PIPE1m,
                        MMU_THDM_DB_QUEUE_CONFIG_A_PIPE2m,
                        MMU_THDM_DB_QUEUE_CONFIG_A_PIPE3m,
                        MMU_THDM_DB_QUEUE_CONFIG_B_PIPE0m,
                        MMU_THDM_DB_QUEUE_CONFIG_B_PIPE1m,
                        MMU_THDM_DB_QUEUE_CONFIG_B_PIPE2m,
                        MMU_THDM_DB_QUEUE_CONFIG_B_PIPE3m,
                        MMU_THDM_DB_QUEUE_CONFIG_C_PIPE0m,
                        MMU_THDM_DB_QUEUE_CONFIG_C_PIPE1m,
                        MMU_THDM_DB_QUEUE_CONFIG_C_PIPE2m,
                        MMU_THDM_DB_QUEUE_CONFIG_C_PIPE3m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE0m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE1m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE2m,
                        MMU_THDM_DB_QUEUE_CONFIG_PIPE3m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE0_PIPE1m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE1_PIPE2m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE1_PIPE3m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE2_PIPE1m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE3_PIPE2m,
                        MMU_THDM_DB_QUEUE_COUNT_XPE3_PIPE3m,
                        MMU_THDM_DB_QUEUE_OFFSET_A_PIPE0m,
                        MMU_THDM_DB_QUEUE_OFFSET_A_PIPE1m,
                        MMU_THDM_DB_QUEUE_OFFSET_A_PIPE2m,
                        MMU_THDM_DB_QUEUE_OFFSET_A_PIPE3m,
                        MMU_THDM_DB_QUEUE_OFFSET_B_PIPE0m,
                        MMU_THDM_DB_QUEUE_OFFSET_B_PIPE1m,
                        MMU_THDM_DB_QUEUE_OFFSET_B_PIPE2m,
                        MMU_THDM_DB_QUEUE_OFFSET_B_PIPE3m,
                        MMU_THDM_DB_QUEUE_OFFSET_C_PIPE0m,
                        MMU_THDM_DB_QUEUE_OFFSET_C_PIPE1m,
                        MMU_THDM_DB_QUEUE_OFFSET_C_PIPE2m,
                        MMU_THDM_DB_QUEUE_OFFSET_C_PIPE3m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE0m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE1m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE2m,
                        MMU_THDM_DB_QUEUE_OFFSET_PIPE3m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE0_PIPE1m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE1_PIPE2m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE1_PIPE3m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE2_PIPE1m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE3_PIPE2m,
                        MMU_THDM_DB_QUEUE_RESUME_XPE3_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE0_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE1_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE1_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE2_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE3_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_BST_XPE3_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE0m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE0m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE0m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE3m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE2m,
                        MMU_THDM_MCQE_PORTSP_CONFIG_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE0_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE1_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE1_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE2_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE3_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_BST_XPE3_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE0_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE1_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE1_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE2_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE3_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_COUNT_XPE3_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_OFFSET_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE0_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE1_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE1_PIPE3m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE2_PIPE1m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE3_PIPE2m,
                        MMU_THDM_MCQE_QUEUE_RESUME_XPE3_PIPE3m,
                        MMU_THDU_BST_PORT_XPE0_PIPE1m,
                        MMU_THDU_BST_PORT_XPE1_PIPE2m,
                        MMU_THDU_BST_PORT_XPE1_PIPE3m,
                        MMU_THDU_BST_PORT_XPE2_PIPE1m,
                        MMU_THDU_BST_PORT_XPE3_PIPE2m,
                        MMU_THDU_BST_PORT_XPE3_PIPE3m,
                        MMU_THDU_BST_QGROUP_XPE0_PIPE1m,
                        MMU_THDU_BST_QGROUP_XPE1_PIPE2m,
                        MMU_THDU_BST_QGROUP_XPE1_PIPE3m,
                        MMU_THDU_BST_QGROUP_XPE2_PIPE1m,
                        MMU_THDU_BST_QGROUP_XPE3_PIPE2m,
                        MMU_THDU_BST_QGROUP_XPE3_PIPE3m,
                        MMU_THDU_BST_QUEUE_XPE0_PIPE1m,
                        MMU_THDU_BST_QUEUE_XPE1_PIPE2m,
                        MMU_THDU_BST_QUEUE_XPE1_PIPE3m,
                        MMU_THDU_BST_QUEUE_XPE2_PIPE1m,
                        MMU_THDU_BST_QUEUE_XPE3_PIPE2m,
                        MMU_THDU_BST_QUEUE_XPE3_PIPE3m,
                        MMU_THDU_CONFIG_PORT0_PIPE0m,
                        MMU_THDU_CONFIG_PORT0_PIPE1m,
                        MMU_THDU_CONFIG_PORT0_PIPE2m,
                        MMU_THDU_CONFIG_PORT0_PIPE3m,
                        MMU_THDU_CONFIG_PORT1_PIPE0m,
                        MMU_THDU_CONFIG_PORT1_PIPE1m,
                        MMU_THDU_CONFIG_PORT1_PIPE2m,
                        MMU_THDU_CONFIG_PORT1_PIPE3m,
                        MMU_THDU_CONFIG_PORT_PIPE0m,
                        MMU_THDU_CONFIG_PORT_PIPE1m,
                        MMU_THDU_CONFIG_PORT_PIPE2m,
                        MMU_THDU_CONFIG_PORT_PIPE3m,
                        MMU_THDU_CONFIG_QGROUP0_PIPE0m,
                        MMU_THDU_CONFIG_QGROUP0_PIPE1m,
                        MMU_THDU_CONFIG_QGROUP0_PIPE2m,
                        MMU_THDU_CONFIG_QGROUP0_PIPE3m,
                        MMU_THDU_CONFIG_QGROUP1_PIPE0m,
                        MMU_THDU_CONFIG_QGROUP1_PIPE1m,
                        MMU_THDU_CONFIG_QGROUP1_PIPE2m,
                        MMU_THDU_CONFIG_QGROUP1_PIPE3m,
                        MMU_THDU_CONFIG_QGROUP_PIPE0m,
                        MMU_THDU_CONFIG_QGROUP_PIPE1m,
                        MMU_THDU_CONFIG_QGROUP_PIPE2m,
                        MMU_THDU_CONFIG_QGROUP_PIPE3m,
                        MMU_THDU_CONFIG_QUEUE0_PIPE0m,
                        MMU_THDU_CONFIG_QUEUE0_PIPE1m,
                        MMU_THDU_CONFIG_QUEUE0_PIPE2m,
                        MMU_THDU_CONFIG_QUEUE0_PIPE3m,
                        MMU_THDU_CONFIG_QUEUE1_PIPE0m,
                        MMU_THDU_CONFIG_QUEUE1_PIPE1m,
                        MMU_THDU_CONFIG_QUEUE1_PIPE2m,
                        MMU_THDU_CONFIG_QUEUE1_PIPE3m,
                        MMU_THDU_CONFIG_QUEUE_PIPE0m,
                        MMU_THDU_CONFIG_QUEUE_PIPE1m,
                        MMU_THDU_CONFIG_QUEUE_PIPE2m,
                        MMU_THDU_CONFIG_QUEUE_PIPE3m,
                        MMU_THDU_COUNTER_PORT_XPE0_PIPE1m,
                        MMU_THDU_COUNTER_PORT_XPE1_PIPE2m,
                        MMU_THDU_COUNTER_PORT_XPE1_PIPE3m,
                        MMU_THDU_COUNTER_PORT_XPE2_PIPE1m,
                        MMU_THDU_COUNTER_PORT_XPE3_PIPE2m,
                        MMU_THDU_COUNTER_PORT_XPE3_PIPE3m,
                        MMU_THDU_COUNTER_QGROUP_XPE0_PIPE1m,
                        MMU_THDU_COUNTER_QGROUP_XPE1_PIPE2m,
                        MMU_THDU_COUNTER_QGROUP_XPE1_PIPE3m,
                        MMU_THDU_COUNTER_QGROUP_XPE2_PIPE1m,
                        MMU_THDU_COUNTER_QGROUP_XPE3_PIPE2m,
                        MMU_THDU_COUNTER_QGROUP_XPE3_PIPE3m,
                        MMU_THDU_COUNTER_QUEUE_XPE0_PIPE1m,
                        MMU_THDU_COUNTER_QUEUE_XPE1_PIPE2m,
                        MMU_THDU_COUNTER_QUEUE_XPE1_PIPE3m,
                        MMU_THDU_COUNTER_QUEUE_XPE2_PIPE1m,
                        MMU_THDU_COUNTER_QUEUE_XPE3_PIPE2m,
                        MMU_THDU_COUNTER_QUEUE_XPE3_PIPE3m,
                        MMU_THDU_OFFSET_QGROUP0_PIPE0m,
                        MMU_THDU_OFFSET_QGROUP0_PIPE1m,
                        MMU_THDU_OFFSET_QGROUP0_PIPE2m,
                        MMU_THDU_OFFSET_QGROUP0_PIPE3m,
                        MMU_THDU_OFFSET_QGROUP1_PIPE0m,
                        MMU_THDU_OFFSET_QGROUP1_PIPE1m,
                        MMU_THDU_OFFSET_QGROUP1_PIPE2m,
                        MMU_THDU_OFFSET_QGROUP1_PIPE3m,
                        MMU_THDU_OFFSET_QGROUP_PIPE0m,
                        MMU_THDU_OFFSET_QGROUP_PIPE1m,
                        MMU_THDU_OFFSET_QGROUP_PIPE2m,
                        MMU_THDU_OFFSET_QGROUP_PIPE3m,
                        MMU_THDU_OFFSET_QUEUE0_PIPE0m,
                        MMU_THDU_OFFSET_QUEUE0_PIPE1m,
                        MMU_THDU_OFFSET_QUEUE0_PIPE2m,
                        MMU_THDU_OFFSET_QUEUE0_PIPE3m,
                        MMU_THDU_OFFSET_QUEUE1_PIPE0m,
                        MMU_THDU_OFFSET_QUEUE1_PIPE1m,
                        MMU_THDU_OFFSET_QUEUE1_PIPE2m,
                        MMU_THDU_OFFSET_QUEUE1_PIPE3m,
                        MMU_THDU_OFFSET_QUEUE_PIPE0m,
                        MMU_THDU_OFFSET_QUEUE_PIPE1m,
                        MMU_THDU_OFFSET_QUEUE_PIPE2m,
                        MMU_THDU_OFFSET_QUEUE_PIPE3m,
                        MMU_THDU_Q_TO_QGRP_MAP0_PIPE0m,
                        MMU_THDU_Q_TO_QGRP_MAP0_PIPE1m,
                        MMU_THDU_Q_TO_QGRP_MAP0_PIPE2m,
                        MMU_THDU_Q_TO_QGRP_MAP0_PIPE3m,
                        MMU_THDU_Q_TO_QGRP_MAP1_PIPE0m,
                        MMU_THDU_Q_TO_QGRP_MAP1_PIPE1m,
                        MMU_THDU_Q_TO_QGRP_MAP1_PIPE2m,
                        MMU_THDU_Q_TO_QGRP_MAP1_PIPE3m,
                        MMU_THDU_Q_TO_QGRP_MAP2_PIPE0m,
                        MMU_THDU_Q_TO_QGRP_MAP2_PIPE1m,
                        MMU_THDU_Q_TO_QGRP_MAP2_PIPE2m,
                        MMU_THDU_Q_TO_QGRP_MAP2_PIPE3m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE0m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE1m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE2m,
                        MMU_THDU_Q_TO_QGRP_MAP_PIPE3m,
                        MMU_THDU_RESUME_PORT0_PIPE0m,
                        MMU_THDU_RESUME_PORT0_PIPE1m,
                        MMU_THDU_RESUME_PORT0_PIPE2m,
                        MMU_THDU_RESUME_PORT0_PIPE3m,
                        MMU_THDU_RESUME_PORT1_PIPE0m,
                        MMU_THDU_RESUME_PORT1_PIPE1m,
                        MMU_THDU_RESUME_PORT1_PIPE2m,
                        MMU_THDU_RESUME_PORT1_PIPE3m,
                        MMU_THDU_RESUME_PORT2_PIPE0m,
                        MMU_THDU_RESUME_PORT2_PIPE1m,
                        MMU_THDU_RESUME_PORT2_PIPE2m,
                        MMU_THDU_RESUME_PORT2_PIPE3m,
                        MMU_THDU_RESUME_PORT_PIPE0m,
                        MMU_THDU_RESUME_PORT_PIPE1m,
                        MMU_THDU_RESUME_PORT_PIPE2m,
                        MMU_THDU_RESUME_PORT_PIPE3m,
                        MMU_THDU_RESUME_QGROUP_XPE0_PIPE1m,
                        MMU_THDU_RESUME_QGROUP_XPE1_PIPE2m,
                        MMU_THDU_RESUME_QGROUP_XPE1_PIPE3m,
                        MMU_THDU_RESUME_QGROUP_XPE2_PIPE1m,
                        MMU_THDU_RESUME_QGROUP_XPE3_PIPE2m,
                        MMU_THDU_RESUME_QGROUP_XPE3_PIPE3m,
                        MMU_THDU_RESUME_QUEUE_XPE0_PIPE1m,
                        MMU_THDU_RESUME_QUEUE_XPE1_PIPE2m,
                        MMU_THDU_RESUME_QUEUE_XPE1_PIPE3m,
                        MMU_THDU_RESUME_QUEUE_XPE2_PIPE1m,
                        MMU_THDU_RESUME_QUEUE_XPE3_PIPE2m,
                        MMU_THDU_RESUME_QUEUE_XPE3_PIPE3m,
                        MMU_UCQDB_X_XPE0m,
                        MMU_UCQDB_X_XPE1m,
                        MMU_UCQDB_X_XPE2m,
                        MMU_UCQDB_X_XPE3m,
                        MMU_UCQDB_Y_XPE0m,
                        MMU_UCQDB_Y_XPE1m,
                        MMU_UCQDB_Y_XPE2m,
                        MMU_UCQDB_Y_XPE3m,
                        MMU_WRED_AVG_QSIZE_XPE0_PIPE1m,
                        MMU_WRED_AVG_QSIZE_XPE1_PIPE2m,
                        MMU_WRED_AVG_QSIZE_XPE1_PIPE3m,
                        MMU_WRED_AVG_QSIZE_XPE2_PIPE1m,
                        MMU_WRED_AVG_QSIZE_XPE3_PIPE2m,
                        MMU_WRED_AVG_QSIZE_XPE3_PIPE3m,
                        MMU_WRED_CONFIG_XPE0_PIPE1m,
                        MMU_WRED_CONFIG_XPE1_PIPE2m,
                        MMU_WRED_CONFIG_XPE1_PIPE3m,
                        MMU_WRED_CONFIG_XPE2_PIPE1m,
                        MMU_WRED_CONFIG_XPE3_PIPE2m,
                        MMU_WRED_CONFIG_XPE3_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE0_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE1_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE2_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_MARK_XPE3_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE0_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE1_PIPE3m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE2_PIPE1m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE2m,
                        MMU_WRED_PORT_SP_DROP_THD_XPE3_PIPE3m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE0_PIPE1m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE2m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE1_PIPE3m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE2_PIPE1m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE2m,
                        MMU_WRED_PORT_SP_SHARED_COUNT_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_0_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_1_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_DROP_THD_MARK_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_FROM_REMOTE_XPE3_PIPE3m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE0_PIPE1m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE1_PIPE2m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE1_PIPE3m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE2_PIPE1m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE3_PIPE2m,
                        MMU_WRED_UC_QUEUE_TOTAL_COUNT_XPE3_PIPE3m,
                        Q_SCHED_L0_ACCUM_COMP_MEM_PIPE0m,
                        Q_SCHED_L0_ACCUM_COMP_MEM_PIPE1m,
                        Q_SCHED_L0_ACCUM_COMP_MEM_PIPE2m,
                        Q_SCHED_L0_ACCUM_COMP_MEM_PIPE3m,
                        Q_SCHED_L0_CREDIT_MEM_PIPE0m,
                        Q_SCHED_L0_CREDIT_MEM_PIPE1m,
                        Q_SCHED_L0_CREDIT_MEM_PIPE2m,
                        Q_SCHED_L0_CREDIT_MEM_PIPE3m,
                        Q_SCHED_L0_WEIGHT_MEM_PIPE0m,
                        Q_SCHED_L0_WEIGHT_MEM_PIPE1m,
                        Q_SCHED_L0_WEIGHT_MEM_PIPE2m,
                        Q_SCHED_L0_WEIGHT_MEM_PIPE3m,
                        Q_SCHED_L1_ACCUM_COMP_MEM_PIPE0m,
                        Q_SCHED_L1_ACCUM_COMP_MEM_PIPE1m,
                        Q_SCHED_L1_ACCUM_COMP_MEM_PIPE2m,
                        Q_SCHED_L1_ACCUM_COMP_MEM_PIPE3m,
                        Q_SCHED_L1_CREDIT_MEM_PIPE0m,
                        Q_SCHED_L1_CREDIT_MEM_PIPE1m,
                        Q_SCHED_L1_CREDIT_MEM_PIPE2m,
                        Q_SCHED_L1_CREDIT_MEM_PIPE3m,
                        Q_SCHED_L1_WEIGHT_MEM_PIPE0m,
                        Q_SCHED_L1_WEIGHT_MEM_PIPE1m,
                        Q_SCHED_L1_WEIGHT_MEM_PIPE2m,
                        Q_SCHED_L1_WEIGHT_MEM_PIPE3m,
                        Q_SCHED_L2_ACCUM_COMP_MEM_PIPE0m,
                        Q_SCHED_L2_ACCUM_COMP_MEM_PIPE1m,
                        Q_SCHED_L2_ACCUM_COMP_MEM_PIPE2m,
                        Q_SCHED_L2_ACCUM_COMP_MEM_PIPE3m,
                        Q_SCHED_L2_CREDIT_MEM_PIPE0m,
                        Q_SCHED_L2_CREDIT_MEM_PIPE1m,
                        Q_SCHED_L2_CREDIT_MEM_PIPE2m,
                        Q_SCHED_L2_CREDIT_MEM_PIPE3m,
                        REPLICATION_FIFO_BANK1_XPE0m,
                        REPLICATION_FIFO_BANK1_XPE1m,
                        REPLICATION_FIFO_BANK1_XPE2m,
                        REPLICATION_FIFO_BANK1_XPE3m,
                        RQE_FREE_LIST_XPE0m,
                        RQE_FREE_LIST_XPE1m,
                        RQE_FREE_LIST_XPE2m,
                        RQE_FREE_LIST_XPE3m,
                        RQE_LINK_LIST_XPE0m,
                        RQE_LINK_LIST_XPE1m,
                        RQE_LINK_LIST_XPE2m,
                        RQE_LINK_LIST_XPE3m,
                        STATS_INTF_QUEUE_LISTm,
                        TDM_CALENDAR0_PIPE0m,
                        TDM_CALENDAR0_PIPE1m,
                        TDM_CALENDAR0_PIPE2m,
                        TDM_CALENDAR0_PIPE3m,
                        TDM_CALENDAR1_PIPE0m,
                        TDM_CALENDAR1_PIPE1m,
                        TDM_CALENDAR1_PIPE2m,
                        TDM_CALENDAR1_PIPE3m,
                        THDI_PORT_PG_BST_XPE0_PIPE3m,
                        THDI_PORT_PG_BST_XPE1_PIPE3m,
                        THDI_PORT_PG_BST_XPE2_PIPE1m,
                        THDI_PORT_PG_BST_XPE2_PIPE2m,
                        THDI_PORT_PG_BST_XPE3_PIPE1m,
                        THDI_PORT_PG_BST_XPE3_PIPE2m,
                        THDI_PORT_PG_CNTRS_RT1_XPE0_PIPE3m,
                        THDI_PORT_PG_CNTRS_RT1_XPE1_PIPE3m,
                        THDI_PORT_PG_CNTRS_RT1_XPE2_PIPE1m,
                        THDI_PORT_PG_CNTRS_RT1_XPE2_PIPE2m,
                        THDI_PORT_PG_CNTRS_RT1_XPE3_PIPE1m,
                        THDI_PORT_PG_CNTRS_RT1_XPE3_PIPE2m,
                        THDI_PORT_PG_CNTRS_RT2_XPE0_PIPE3m,
                        THDI_PORT_PG_CNTRS_RT2_XPE1_PIPE3m,
                        THDI_PORT_PG_CNTRS_RT2_XPE2_PIPE1m,
                        THDI_PORT_PG_CNTRS_RT2_XPE2_PIPE2m,
                        THDI_PORT_PG_CNTRS_RT2_XPE3_PIPE1m,
                        THDI_PORT_PG_CNTRS_RT2_XPE3_PIPE2m,
                        THDI_PORT_PG_CNTRS_SH1_XPE0_PIPE3m,
                        THDI_PORT_PG_CNTRS_SH1_XPE1_PIPE3m,
                        THDI_PORT_PG_CNTRS_SH1_XPE2_PIPE1m,
                        THDI_PORT_PG_CNTRS_SH1_XPE2_PIPE2m,
                        THDI_PORT_PG_CNTRS_SH1_XPE3_PIPE1m,
                        THDI_PORT_PG_CNTRS_SH1_XPE3_PIPE2m,
                        THDI_PORT_PG_CNTRS_SH2_XPE0_PIPE3m,
                        THDI_PORT_PG_CNTRS_SH2_XPE1_PIPE3m,
                        THDI_PORT_PG_CNTRS_SH2_XPE2_PIPE1m,
                        THDI_PORT_PG_CNTRS_SH2_XPE2_PIPE2m,
                        THDI_PORT_PG_CNTRS_SH2_XPE3_PIPE1m,
                        THDI_PORT_PG_CNTRS_SH2_XPE3_PIPE2m,
                        THDI_PORT_PG_CONFIG_PIPE0m,
                        THDI_PORT_PG_CONFIG_PIPE1m,
                        THDI_PORT_PG_CONFIG_PIPE2m,
                        THDI_PORT_PG_CONFIG_PIPE3m,
                        THDI_PORT_SP_BST_XPE0_PIPE3m,
                        THDI_PORT_SP_BST_XPE1_PIPE3m,
                        THDI_PORT_SP_BST_XPE2_PIPE1m,
                        THDI_PORT_SP_BST_XPE2_PIPE2m,
                        THDI_PORT_SP_BST_XPE3_PIPE1m,
                        THDI_PORT_SP_BST_XPE3_PIPE2m,
                        THDI_PORT_SP_CNTRS_RT_XPE0_PIPE3m,
                        THDI_PORT_SP_CNTRS_RT_XPE1_PIPE3m,
                        THDI_PORT_SP_CNTRS_RT_XPE2_PIPE1m,
                        THDI_PORT_SP_CNTRS_RT_XPE2_PIPE2m,
                        THDI_PORT_SP_CNTRS_RT_XPE3_PIPE1m,
                        THDI_PORT_SP_CNTRS_RT_XPE3_PIPE2m,
                        THDI_PORT_SP_CNTRS_SH_XPE0_PIPE3m,
                        THDI_PORT_SP_CNTRS_SH_XPE1_PIPE3m,
                        THDI_PORT_SP_CNTRS_SH_XPE2_PIPE1m,
                        THDI_PORT_SP_CNTRS_SH_XPE2_PIPE2m,
                        THDI_PORT_SP_CNTRS_SH_XPE3_PIPE1m,
                        THDI_PORT_SP_CNTRS_SH_XPE3_PIPE2m,
                        THDI_PORT_SP_CONFIG0_PIPE0m,
                        THDI_PORT_SP_CONFIG0_PIPE1m,
                        THDI_PORT_SP_CONFIG0_PIPE2m,
                        THDI_PORT_SP_CONFIG0_PIPE3m,
                        THDI_PORT_SP_CONFIG1_PIPE0m,
                        THDI_PORT_SP_CONFIG1_PIPE1m,
                        THDI_PORT_SP_CONFIG1_PIPE2m,
                        THDI_PORT_SP_CONFIG1_PIPE3m,
                        THDI_PORT_SP_CONFIG2_PIPE0m,
                        THDI_PORT_SP_CONFIG2_PIPE1m,
                        THDI_PORT_SP_CONFIG2_PIPE2m,
                        THDI_PORT_SP_CONFIG2_PIPE3m,
                        THDI_PORT_SP_CONFIG_PIPE0m,
                        THDI_PORT_SP_CONFIG_PIPE1m,
                        THDI_PORT_SP_CONFIG_PIPE2m,
                        THDI_PORT_SP_CONFIG_PIPE3m
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
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    memidx = meminfo->base & ecc_par_p->memidx_mask;

    unique_acc_types[0] = ecc_par_p->acc_type_unique[0];
    unique_acc_types[1] = ecc_par_p->acc_type_unique[1];
    unique_acc_types[2] = ecc_par_p->acc_type_unique[2];
    unique_acc_types[3] = ecc_par_p->acc_type_unique[3];

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

    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    soc_cm_get_id (unit, &dev_id, &rev_id);

    cli_out("Device ID = 0x%04x, Rev ID = %0d\n", dev_id, rev_id);

    if (dev_id == BCM56960_DEVICE_ID || dev_id == BCM56860_DEVICE_ID ||
        dev_id == BCM56970_DEVICE_ID) {
        ecc_par_p->sbus_v4 = 1;
        ecc_par_p->memidx_mask = SBUS_V4_MMU_MEMIDX_MASK;
        ecc_par_p->num_unique_acc_types = TH_NUM_UNIQUE_ACC_TYPES;
        ecc_par_p->num_acc_types = TH_NUM_VALID_ACC_TYPES;
        ecc_par_p->acc_type_duplicate = TH_ACC_TYPE_DUPLICATE;
        ecc_par_p->acc_type_single = TH_ACC_TYPE_SINGLE;
        ecc_par_p->acc_type_data_split = TH_ACC_TYPE_DATA_SPLIT;
        ecc_par_p->acc_type_addr_split_dist = TH_ACC_TYPE_ADDR_SPLIT_DIST;
        ecc_par_p->acc_type_addr_split_split = TH_ACC_TYPE_ADDR_SPLIT_SPLIT;

        ecc_par_p->acc_type_unique = (uint32*)
                                    sal_alloc(ecc_par_p->num_unique_acc_types *
                                                        sizeof(uint32),
                                                       "acc_type_unique");
        ecc_par_p->acc_type_unique[0] = TH_ACC_TYPE_UNIQUE_PIPE0;
        ecc_par_p->acc_type_unique[1] = TH_ACC_TYPE_UNIQUE_PIPE1;
        ecc_par_p->acc_type_unique[2] = TH_ACC_TYPE_UNIQUE_PIPE2;
        ecc_par_p->acc_type_unique[3] = TH_ACC_TYPE_UNIQUE_PIPE3;

        ecc_par_p->memidx_mask = SBUS_V4_MMU_MEMIDX_MASK;
        ecc_par_p->get_valid_unique_acc_types = &th_get_valid_unique_acc_types;

    } else {
        cli_out("\nUnsupported chip");
        ecc_par_p->test_fail = 1;
    }
}

static int
is_acc_type_unique(int unit, soc_mem_t mem)
{
    int rv = SOC_E_NONE;
    int i;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    for (i = 0; i < ecc_par_p->num_unique_acc_types; i++) {
        if (SOC_MEM_ACC_TYPE(unit, mem) == ecc_par_p->acc_type_unique[i]) {
            rv = 1;
        }
    }

    return rv;
}


static int
do_not_test(int unit, soc_mem_t mem)
{
    int i;
    int rv = SOC_E_NONE;
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
    int rv = SOC_E_NONE;
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
parity_field_exists(int unit, soc_mem_t mem) {
    return (SOC_MEM_FIELD_VALID(unit, mem, PARITYf) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITYf) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_Af) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_Bf) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_LOWERf) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_UPPERf) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_P0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_P1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_P2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PARITY_P3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PBM_PARITY_P0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PBM_PARITY_P1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PBM_PARITY_P2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_PBM_PARITY_P3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_EGR_VLAN_STG_PARITY_P0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_EGR_VLAN_STG_PARITY_P1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_EGR_VLAN_STG_PARITY_P2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, EVEN_EGR_VLAN_STG_PARITY_P3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY_0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY_1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY_2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY_3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, PARITY_4f));
}

static int
ecc_field_exists(int unit, soc_mem_t mem) {
    return (SOC_MEM_FIELD_VALID(unit, mem, ECCf) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC_0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC_1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC_2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC_3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC_4f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCPf) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP4f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP5f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP6f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP7f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP_0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP_1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP_2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP_3f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECCP_4f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC0f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC1f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC2f) ||
            SOC_MEM_FIELD_VALID(unit, mem, ECC3f));
}

static int
test_mem(int unit, soc_mem_t mem)
{
    int rv = SOC_E_NONE;
    uint32 flags = 0;

    if (SOC_MEM_IS_VALID(unit, mem)) {
        flags = SOC_MEM_INFO(unit, mem).flags;
        if ((!((flags & SOC_MEM_FLAG_AGGR) || (flags & SOC_MEM_FLAG_READONLY)))
                && (do_not_test(unit, mem) == 0)) {
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
    int rv = SOC_E_NONE;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);

    if ((test_mem(unit, mem) == 1)
                && (is_acc_type_unique(unit, mem) == 1)) {
        for (i = 0; i < NUM_SOC_MEM; i++) {
            if (test_mem(unit, i) == 1) {
                if (meminfo->base == SOC_MEM_INFO(unit, i).base) {
                    if ((SOC_MEM_ACC_TYPE(unit, i)
                                    == ecc_par_p->acc_type_duplicate)
                        || (SOC_MEM_ACC_TYPE(unit, i)
                                            == ecc_par_p->acc_type_data_split)
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
    int rv = SOC_E_NONE;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    if ((SOC_MEM_ACC_TYPE(unit, mem) == ecc_par_p->acc_type_duplicate) &&
        (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL)) {
        rv = 1;
    }

    return rv;
}

static void
ecc_par_parse_test_params(int unit, args_t *a)
{
    int copyno;
    parse_table_t parse_table;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    const char tr902_test_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nDocumentation too long to be displayed with -pedantic compiler\n";
#else
    "This test checks the parity and ECC based error detection and correction\n"
    "circuits. It fills memories with random values. It injects a single bit error\n"
    "for parity error detection and ECC error correction. For those memory with ECC\n"
    "protection, it injects a two bit error for ECC error detection. The test goes\n"
    "through one entry per memory by default. The user may use the following options\n"
    "to test one memory instead of all memories and to test all entries instead of\n"
    "one entry.\n"
    "\n"
    "Memory: Name of the memory to be tested; default is all\n"
    "Entry:  Number of entry to be tested; default is one\n";
#endif

    ecc_par_p->mem_str = sal_strdup("all");
    ecc_par_p->single_entry = TRUE;

    /*Parse CLI opts */
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table, "Memory", PQ_STRING | PQ_DFL,
                    0, &(ecc_par_p->mem_str), NULL);
    parse_table_add(&parse_table, "SingleEntry", PQ_BOOL | PQ_DFL,
                    0, &(ecc_par_p->single_entry), NULL);

    if (ecc_par_p->single_entry) {
        ecc_par_p->test_type = SER_SINGLE_INDEX;
    } else {
        ecc_par_p->test_type = SER_ALL_INDEXES;
    }

    if (parse_arg_eq(a, &parse_table) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", tr902_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        ecc_par_p->test_fail = 1;
        parse_arg_eq_done(&parse_table);
    } else {
        cli_out("-----------------\n");
        cli_out(" TEST PARAMETERS \n");
        cli_out("-----------------\n");
        cli_out("Memory = %s\n", ecc_par_p->mem_str);
        cli_out("Entry  = %s\n", ecc_par_p->single_entry ? "single" : "all");
        cli_out("-----------------\n");
    }

    if ((sal_strstr(ecc_par_p->mem_str, "all") == NULL) &&
        (parse_memory_name(unit, &ecc_par_p->mem, ecc_par_p->mem_str, &copyno, 0) < 0)) {
        test_error(unit, "Memory \"%s\" is invalid\n", ecc_par_p->mem_str);
        ecc_par_p->test_fail = 1;
    }
}

static void
eccmask_get(int unit, soc_mem_t mem, uint32 *buf)
{
    int               f, b, start, end;
    soc_mem_info_t    *memp;
    soc_field_info_t  *fieldp;
    uint32            tmp;

    memp = &SOC_MEM_INFO(unit, mem);
    sal_memset(buf, 0, sizeof(*buf) * BYTES2WORDS(memp->bytes));

    for (f = 0; f < memp->nFields; f++) {
        fieldp = &(memp->fields[f]);

        if (fieldp->flags & SOCF_RES) {
            continue;
        }

        if ((sal_strstr(SOC_FIELD_NAME(unit, fieldp->field), "ECC") != NULL) ||
            (sal_strstr(SOC_FIELD_NAME(unit, fieldp->field), "PARITY") != NULL) ||
            (sal_strstr(SOC_FIELD_NAME(unit, fieldp->field), "RESERVED") != NULL)) {
            continue;
        }

        start = fieldp->bp;
        end = fieldp->bp + fieldp->len - 1;

        for (b = start / 32; b <= end / 32; b++) {
            tmp = -1;

            if (b == start / 32) {
                tmp &= -1 << (start % 32);
            }

            if (b == end / 32) {
                tmp &= (1 << (end % 32) << 1) - 1;
            }

            buf[FIX_MEM_ORDER_E(b, memp)] |= tmp;
        }
    }
}

static void
write_one_mem(int unit, soc_mem_t mem)
{
    uint32 i, j;
    uint32 seed;
    uint32 *wr_data;
    uint32 entry_words;
    uint32 flags = SOC_MEM_DONT_USE_CACHE | SOC_MEM_DONT_CONVERT_XY2DM;
    uint32 meminfo_flags;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    meminfo_flags = meminfo->flags;
    meminfo->flags &= (~SOC_MEM_FLAG_CAM);

    for (i = soc_mem_index_min(unit, mem);
         i < (soc_mem_index_max(unit, mem) + 1);
         i++) {

        entry_words = soc_mem_entry_words(unit, mem);
        wr_data = (uint32*)sal_alloc(entry_words * sizeof(uint32), "wr_data");

        for (j = 0; j < entry_words; j++) {
            seed = i * NUM_SOC_MEM + j;
            wr_data[j] = RNUM(seed) & ecc_par_p->mask_array[mem][j];
        }

        (void) soc_mem_write_extended(unit, flags, mem, COPYNO_ALL, i, wr_data);

        sal_free(wr_data);
        if (ecc_par_p->single_entry == 1) break;
    }

    meminfo->flags = meminfo_flags;
}

static void
write_all_mems(int unit)
{
    int i;
    soc_mem_t mem;
    uint32 acc_type;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    /*init soc*/
    if (SOC_FAILURE(soc_reset_init(unit))) {
        cli_out("Error during soc reset.  Aborting.\n");
        ecc_par_p->test_fail = 1;
    }

    /*init misc*/
    if (SOC_FAILURE(soc_misc_init(unit))) {
        cli_out("Error during misc init.  Aborting.\n");
        ecc_par_p->test_fail = 1;
    }

    /*init mmu*/
    if (SOC_FAILURE(soc_mmu_init(unit))) {
        cli_out("Error during mmu init.  Aborting.\n");
        ecc_par_p->test_fail = 1;
    }

    for (i = 0; i < NUM_SOC_MEM; i++) {
        if (sal_strstr(ecc_par_p->mem_str, "all") == NULL)
            mem = ecc_par_p->mem;
        else
            mem = (soc_mem_t)(i);

        if (test_mem(unit, mem) == 1 && duplicate_mem(unit, mem) == 0 &&
            (parity_field_exists(unit, mem) || ecc_field_exists(unit, mem))) {
            acc_type = SOC_MEM_ACC_TYPE(unit, mem);
            cli_out("Writing %s, acc_type = %0d, base = 0x%08x\n",
                    SOC_MEM_NAME(unit, mem), acc_type,
                    SOC_MEM_INFO(unit, mem).base);
            write_one_mem(unit, mem);
        }

        if (sal_strstr(ecc_par_p->mem_str, "all") == NULL) break;
    }
}

static void
read_and_check_one_mem(int unit, soc_mem_t mem)
{
    uint32 i, j, k;
    uint32 seed;
    uint32 *rd_data;
    uint32 exp_word;
    uint32 entry_words;
    uint32 flags = SOC_MEM_DONT_USE_CACHE | SOC_MEM_DONT_CONVERT_XY2DM;
    uint32 use_unique_access_types = 0;
    uint32 mask;
    uint32 eccmask[SOC_MAX_MEM_WORDS];
    uint32 meminfo_flags;
    uint32 *valid_acc_types;
    soc_mem_info_t *meminfo = &SOC_MEM_INFO(unit, mem);
    int rv = SOC_E_NONE;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    meminfo_flags = meminfo->flags;
    meminfo->flags &= (~SOC_MEM_FLAG_CAM);

    use_unique_access_types = do_unique_access(unit, mem);
    entry_words = soc_mem_entry_words(unit, mem);

    valid_acc_types = (uint32*) sal_alloc(ecc_par_p->num_unique_acc_types *
                                            sizeof(uint32), "acc_type_unique");

    ecc_par_p->get_valid_unique_acc_types(unit, mem, valid_acc_types);

    for (i = 0; i < ecc_par_p->num_unique_acc_types; i++) {

        if (use_unique_access_types == 0 && i > 0) {
            break;
        } else if (use_unique_access_types == 0 && i == 0) {
            cli_out("Reading and Checking %s\n", SOC_MEM_NAME(unit, mem));
        } else {
            if (valid_acc_types[i] != INVALID_ACC_TYPE) {
                cli_out("Reading and Checking %s for acc_type %0d\n",
                        SOC_MEM_NAME(unit, mem), valid_acc_types[i]);
            } else {
                continue;
            }
        }

        eccmask_get(unit, mem, eccmask);
        for (j = soc_mem_index_min(unit, mem);
            j < (soc_mem_index_max(unit, mem) + 1);
            j++) {

            rd_data = (uint32*)sal_alloc(entry_words * sizeof(uint32),
                                         "rd_data");
            if (use_unique_access_types == 0) {
                rv = soc_mem_read_extended(unit, flags, mem, 0,
                                           COPYNO_ALL, j, rd_data);
            } else {
                rv = soc_mem_pipe_select_read(unit, flags, mem, COPYNO_ALL,
                                              valid_acc_types[i], j, rd_data);
            }
            if (SOC_FAILURE(rv)) {
                test_error(unit, "Memory %s read got NACK\n",
                           SOC_MEM_NAME(unit, mem));
                ecc_par_p->test_fail = 1;
            }

            for (k = 0; k < entry_words; k++) {
                mask = ecc_par_p->mask_array[mem][k] & eccmask[k];
                seed = j * NUM_SOC_MEM + k;
                exp_word = RNUM(seed);
                if ((rd_data[k] & mask) != (exp_word & mask)) {
                    test_error(unit, "Memory %s, index %0d, word %0d, mismatched: exp_word = 0x%08x, rd_data = 0x%08x\n",
                               SOC_MEM_NAME(unit, mem), j, k, exp_word, rd_data[k]);
                    ecc_par_p->test_fail = 1;
                }
            }

            rv = soc_ser_inject_support(unit, mem, 0);
            if ((rv == SOC_E_NONE) && (SOC_MEM_SER_CORRECTION_TYPE(unit, mem) != 0)) {
                SOC_MEM_TEST_SKIP_CACHE_SET(unit, 1);
                if (SOC_FAILURE(soc_mem_parity_control(unit, mem, COPYNO_ALL, FALSE))) {
                    test_error(unit, "Could not disable parity warnings on memory %s\n",
                               SOC_MEM_UFNAME(u, mem));
                    ecc_par_p->test_fail = 1;
                }

                if (ecc_field_exists(unit, mem)) {
                    flags |= SOC_INJECT_ERROR_2BIT_ECC;
                }

                rv = soc_ser_inject_error(unit, flags, mem, 0, -1, j);
                if (SOC_FAILURE(rv)) {
                    test_error(unit, "Failed to inject error to memory: %s\n",
                               SOC_MEM_UFNAME(u, mem));
                    ecc_par_p->test_fail = 1;
                }

                /* Read the memory entry with cache disabled, in order to induce the error */
                flags |= SER_TEST_MEM_F_READ_FUNC_VIEW;
                if (use_unique_access_types == 0) {
                    rv = soc_mem_read_extended(unit, flags, mem, 0,
                                               COPYNO_ALL, j, rd_data);
                } else {
                    rv = soc_mem_pipe_select_read(unit, flags, mem, COPYNO_ALL,
                                                  valid_acc_types[i], j, rd_data);
                }
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit)) {
                    /* Following assumes that in TH, we have injected 2b error if mem
                     * was ECC protected.
                     * For ECC protected mems, if we inject only 1b error, and even if 1b
                     * error reporting is enabled, then we will see ser event and
                     * correction but will not get NACK on 1st read
                     */
                    if (!SOC_FAILURE(rv) && valid_acc_types[i] == 0) {
                        test_error(unit, "Did not receive NACK on 1st Read "
                                         "from memory %s index %d\n",
                                   SOC_MEM_UFNAME(unit, mem), j);
                        ecc_par_p->test_fail = 1;
                    }
                }
#endif /* BCM_TOMAHAWK_SUPPORT */

                SOC_MEM_TEST_SKIP_CACHE_SET(unit, 0);
                if (SOC_FAILURE(soc_mem_parity_restore(unit, mem, COPYNO_ALL))) {
                    test_error(unit, "Could not enable parity warnings on memory %s\n",
                               SOC_MEM_UFNAME(unit, mem));
                    ecc_par_p->test_fail = 1;
                }

                ecc_par_p->total_entries_tested++;
            }
            sal_free(rd_data);
            if (ecc_par_p->single_entry == 1) break;
        }
    }

    ecc_par_p->total_memories_tested++;
    sal_free(valid_acc_types);
    meminfo->flags = meminfo_flags;
}

static void
read_and_check_all_mems(int unit)
{
    int i;
    soc_mem_t mem;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];
    int force_read_through = SOC_MEM_FORCE_READ_THROUGH(unit);
#ifdef INCLUDE_MEM_SCAN
    sal_usecs_t mem_scan_interval = 0;
    int mem_scan_rate = 0, mem_scan_running;
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    sal_usecs_t sram_scan_interval = 0;
    int sram_scan_rate = 0, sram_scan_running;
#endif

    if (!force_read_through) {
       SOC_MEM_FORCE_READ_THROUGH_SET(unit, 1);
    }
    
#ifdef INCLUDE_MEM_SCAN
    mem_scan_running = soc_mem_scan_running (unit, &mem_scan_rate,
                                             &mem_scan_interval);
    if (mem_scan_running > 0) {
        if (soc_mem_scan_stop(unit)) {
            ecc_par_p->test_fail = 1;
        }
    }
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    sram_scan_running = soc_sram_scan_running (unit, &sram_scan_rate,
                                               &sram_scan_interval);
    if (sram_scan_running > 0) {
        if (soc_sram_scan_stop(unit)) {
            ecc_par_p->test_fail = 1;
        }
    }
#endif

    for (i = 0; i < NUM_SOC_MEM; i++) {
        if (sal_strstr(ecc_par_p->mem_str, "all") == NULL)
            mem = ecc_par_p->mem;
        else
            mem = (soc_mem_t)(i);

        if (test_mem(unit, mem) == 1 && duplicate_mem(unit, mem) == 0) {
            if (parity_field_exists(unit, mem) || ecc_field_exists(unit, mem)) {
                read_and_check_one_mem(unit, mem);
            }
        }
        if (sal_strstr(ecc_par_p->mem_str, "all") == NULL) break;
    }

    if (!force_read_through) {
        SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);
    }

    if (SOC_FAILURE(soc_reset_init(unit))) {
        cli_out("Error during soc reset.  Aborting.\n");
        ecc_par_p->test_fail = 1;
    }
    /*init misc*/
    if (SOC_FAILURE(soc_misc_init(unit))) {
        cli_out("Error during misc init.  Aborting.\n");
        ecc_par_p->test_fail = 1;
    }
#ifdef INCLUDE_MEM_SCAN
    if (mem_scan_running) {
        if (soc_mem_scan_start(unit, mem_scan_rate, mem_scan_interval)) {
            ecc_par_p->test_fail = 1;
        }
    }
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
    if (sram_scan_running) {
        if (soc_sram_scan_start(unit, sram_scan_rate, sram_scan_interval)) {
            ecc_par_p->test_fail = 1;
        }
    }
#endif
}

int
ecc_par_test_init(int unit, args_t *a, void **pa)
{
    int i, j;
    soc_mem_t mem;
    uint32 datamask[SOC_MAX_MEM_WORDS];
    uint32 eccmask[SOC_MAX_MEM_WORDS];
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    ecc_par_p = sal_alloc(sizeof(ecc_par_t), "ecc_par_test");
    sal_memset(ecc_par_p, 0, sizeof(ecc_par_t));
    ecc_par_parray[unit] = ecc_par_p;

    ecc_par_p->test_fail = 0;
    ecc_par_p->total_memories_tested = 0;
    ecc_par_p->total_entries_tested = 0;

    ecc_par_parse_test_params(unit, a);

    ecc_par_p->mask_array = (uint32**)sal_alloc(NUM_SOC_MEM * sizeof(uint32*),
                                         "mask_array*");

    for (i = 0; i < NUM_SOC_MEM; i++) {
        ecc_par_p->mask_array[i] = (uint32*)sal_alloc(SOC_MAX_MEM_WORDS *
                                                                sizeof(uint32),
                                                        "mask_array");
    }

    for (i = 0; i < NUM_SOC_MEM; i++) {
        for (j = 0; j < SOC_MAX_MEM_WORDS; j++) {
            ecc_par_p->mask_array[i][j] = 0xffffffff;
        }
    }

    for (i = 0; i < NUM_SOC_MEM; i++) {
        mem = (soc_mem_t)(i);

        if (SOC_MEM_IS_VALID(unit, mem)) {
            soc_mem_datamask_get(unit, mem, datamask);
            eccmask_get(unit, mem, eccmask);
            for (j = 0; j < SOC_MAX_MEM_WORDS; j++) {
                ecc_par_p->mask_array[i][j] = datamask[j] & eccmask[j];
            }
        }
    }

    get_chip_specific_data(unit);

    /** Turn off other Threads **/
    bcm_common_linkscan_enable_set(unit,0);
    soc_counter_stop(unit);

    if (ecc_par_p->test_fail == 1) {
        return BCM_E_FAIL;
    } else {
        return BCM_E_NONE;
    }
}

int
ecc_par_test(int unit, args_t *a, void *pa)
{
    write_all_mems(unit);
    read_and_check_all_mems(unit);
    return 0;
}

int
ecc_par_test_cleanup(int unit, void *pa)
{
    int i;
    ecc_par_t *ecc_par_p = ecc_par_parray[unit];

    cli_out("---------------------------\n");
    if (ecc_par_p->test_fail == 1)
        cli_out("        TEST FAILED        \n");
    else
        cli_out("        TEST PASSED        \n");
    cli_out("---------------------------\n");
    cli_out("Total memories tested = %0d\n", ecc_par_p->total_memories_tested);
    cli_out("Total entries tested  = %0d\n", ecc_par_p->total_entries_tested);
    cli_out("---------------------------\n");

    for (i = 0; i < NUM_SOC_MEM; i++) {
        sal_free(ecc_par_p->mask_array[i]);
    }

    sal_free(ecc_par_p->mask_array);

    sal_free(ecc_par_p->acc_type_unique);
    sal_free(ecc_par_p);
    return 0;
}

#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT2_SUPPORT */

