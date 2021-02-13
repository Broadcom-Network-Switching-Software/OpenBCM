/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * File: arch.h
 */


#ifndef INCLUDED_NLM_ARCH_HEADER
#define INCLUDED_NLM_ARCH_HEADER

#define NLMDEV_MAX_DEV_NUM                      (4)
#define NLMDEV_MAX_DEV_NUM_SCG4                 (2)

#define NLMDEV_MAX_DEV_NUM_11K                  (4)


#define NLMDEV_GET_DEV_ID(address)              ((uint8_t)(((address) >>  23) & 0x3))

#define NLMDEV_NUM_ARRAY_BLOCKS                 (256)
#define NLMDEV_TOTAL_NUM_AC                     (16)
#define NLMDEV_NUM_SUPER_BLOCKS                 (32)


#define NLMDEV_NUM_AB_PER_SUPER_BLOCK           (NLMDEV_NUM_ARRAY_BLOCKS / NLMDEV_NUM_SUPER_BLOCKS)

#define NLMDEV_NUM_SB_PER_AC                    (2)
#define NLMDEV_NUM_AB_PER_AC                    (NLMDEV_NUM_SB_PER_AC * NLMDEV_NUM_AB_PER_SUPER_BLOCK)


#define NLMDEV_TOTAL_NUM_AC_40M                 (8)
#define NLMDEV_NUM_ARRAY_BLOCKS_40M             (128)
#define NLMDEV_NUM_AB_PER_SUPER_BLOCK_40M       (NLMDEV_NUM_ARRAY_BLOCKS_40M/NLMDEV_NUM_SUPER_BLOCKS)
#define NLMDEV_NUM_AB_PER_AC_40M                (NLMDEV_NUM_SB_PER_AC * NLMDEV_NUM_AB_PER_SUPER_BLOCK_40M)


#define NLMDEV_AB_WIDTH_IN_BITS                 (80)

#define NLMDEV_AB_WIDTH_IN_BYTES                (NLMDEV_AB_WIDTH_IN_BITS / 8)

#define NLMDEV_AB_MIN_WIDTH_IN_BITS             (80)

#define NLMDEV_AB_MIN_WIDTH_IN_BYTES            (NLMDEV_AB_MIN_WIDTH_IN_BITS / 8)

#define NLMDEV_AB_DEPTH                         (4096)

#define NLMDEV_MAX_AB_WIDTH_IN_BITS             (640)

#define NLMDEV_MAX_AB_WIDTH_IN_BYTES            (NLMDEV_MAX_AB_WIDTH_IN_BITS / 8)

#define NLMDEV_MAX_NUM_DBA_ENTRIES              (NLMDEV_AB_DEPTH * NLMDEV_NUM_ARRAY_BLOCKS)

#define NLMDEV_NUM_BMRS_PER_AB                  (5)

#define NLMDEV_NUM_80BIT_SEGMENTS_PER_BMR       (4)

#define NLMDEV_NUM_KEYS_PER_INST                (4)
#define NLMDEV_NUM_KEYS                         (NLMDEV_NUM_KEYS_PER_INST)

#define NLMDEV_NUM_KPU                          (4)

#define NLMDEV_NUM_OF_KCR_PER_KEY               (2)
#define NLMDEV_NUM_OF_SEGMENTS_PER_KCR          (5)
#define NLMDEV_NUM_OF_SEGMENTS_PER_KEY          (NLMDEV_NUM_OF_SEGMENTS_PER_KCR * NLMDEV_NUM_OF_KCR_PER_KEY)
#define NLMDEV_MAX_NUM_BYTES_PER_KCR_SEG        (16)

#define NLMDEV_NUM_PARALLEL_SEARCHES_PER_INST   (4)
#define NLMDEV_NUM_PARALLEL_SEARCHES            (NLMDEV_NUM_PARALLEL_SEARCHES_PER_INST)

#define NLM_MAX_NUM_RESULT_PORTS                (4)

#define NLM_MAX_NUM_RESULTS                     (6)

#define NLM_DATA_WIDTH_BYTES                    (10)

#define NLM_EM_RECORD_WIDTH_IN_BYTES            (8)

#define NLM_EM_RECORD_WIDTH_IN_BITS             (64)

#define NLM_EM_HW_INPUT_LEN_IN_BYTES            (10)

#define NLM_EM_HW_OUTPUT_LEN_IN_BYTES           (NLM_EM_HW_INPUT_LEN_IN_BYTES + 2)

#define NLM_MAX_UDA_REQUESTS_PER_CYCLE          (5)

#define NLM_MIN_SRAM_WIDTH_IN_BITS              (32)

#define NLM_MIN_SRAM_WIDTH_IN_BYTES             (NLM_MIN_SRAM_WIDTH_IN_BITS/8)

#define NLM_MAX_SRAM_SB_WIDTH_IN_BITS           (256)

#define NLM_MAX_SRAM_SB_WIDTH_IN_BYTES          (NLM_MAX_SRAM_SB_WIDTH_IN_BITS/8)

#define NLMDEV_NUM_SRAM_SUPER_BLOCKS            (64)

#define NLMDEV_NUM_SRAM_BLOCKS_IN_SB            (8)

#define NLMDEV_SRAM_BASE_ADDR_SHIFT_NUM_BITS    (9)

#define NLMDEV_NUM_ENTRIES_PER_SRAM_BLOCK       (8 * 1024)

#define NLMDEV_TOTAL_NUM_SRAM_BLOCKS            (NLMDEV_NUM_SRAM_SUPER_BLOCKS * NLMDEV_NUM_SRAM_BLOCKS_IN_SB)

#define NLMDEV_TOTAL_NUM_SRAM_ENTRIES           (NLMDEV_TOTAL_NUM_SRAM_BLOCKS * NLMDEV_NUM_ENTRIES_PER_SRAM_BLOCK)

#define NLMDEV_NUM_SRAM_ENTRIES_PER_SB          (NLMDEV_NUM_SRAM_BLOCKS_IN_SB * NLMDEV_NUM_ENTRIES_PER_SRAM_BLOCK)


#define NLMDEV_REG_LEN_IN_BITS                  (80)

#define NLMDEV_REG_LEN_IN_BYTES                 (NLMDEV_REG_LEN_IN_BITS / 8)


#define NLM_MAX_MEM_SIZE                        (0x1000000)

#define NLMDEV_FIB_MAX_PFX_LEN_IN_BITS          (160)
#define NLMDEV_FIB_MAX_PFX_LEN_IN_BYTES         (NLMDEV_FIB_MAX_PFX_LEN_IN_BITS/8)


#define NLMDEV_FIB_MAX_INDEX_RANGE              (0xFFFFFF)


#define NLMDEV_OPCODE_EXT_LCLOPCODE_LPM     (0x5)


typedef enum NlmParallelSrchType_e {
    NLMDEV_PARALLEL_SEARCH_0 = 0,
    NLMDEV_PARALLEL_SEARCH_1,
    NLMDEV_PARALLEL_SEARCH_2,
    NLMDEV_PARALLEL_SEARCH_3,

    NLMDEV_PARALLEL_SEARCH_END  
} NlmDevParallelSrch;


typedef enum NlmKeyType_e {
    NLMDEV_KEY_0 = 0,
    NLMDEV_KEY_1,
    NLMDEV_KEY_2,
    NLMDEV_KEY_3,
    NLMDEV_KEY_END              
} NlmDevKey;


#define NLM_INDEX_SHIFT_VALUE_MAX                       (3)

#define NLM_MAX_CMP_DATA_LEN_BYTES                      (80)



#define NLMDEV_CB_DEPTH                                 (32768)


#define NLMDEV_MAX_NR_OF_CONTEXTS                       (NLMDEV_CB_DEPTH / 8)


#define NLMDEV_CB_WIDTH_IN_BITS                         (80)
#define NLMDEV_CB_WIDTH_IN_BYTES                        (NLMDEV_CB_WIDTH_IN_BITS/8)
#define NLMDEV_CB_KEYLEN                                (640)
#define NLMDEV_CB_KEYLEN_BYTES                          (NLMDEV_CB_KEYLEN/8)
#define NLMDEV_MAX_CB_WRITE_IN_BYTES                    (NLMDEV_CB_KEYLEN_BYTES)


#define NLMDEV_CB_MEMSIZE                               (NLMDEV_CB_DEPTH * NLMDEV_CB_WIDTH_IN_BYTES)


#define NLMDEV_MAX_NUM_LTRS                             (128)

#define NLM_ADDR_SPACE_FOR_REGS_PER_LTR                 (256)

#define NLM_NUM_BITS_FOR_REG_ADDR_SPACE_PER_LTR         (8)


#define NLM_MAX_NUM_SMT                                 (2)
#define NLM_MAX_NUM_LTR_PER_SMT                         (NLMDEV_MAX_NUM_LTRS / 2)
#define NLM_MAX_NUM_PORT                                    (2)
#define NLM_DEFAULT_SMT_NUM                             (0)


#define NLM_RB_NUM_RESULT_REG                           (3)

#define NLM_RB_NUM_RESULTS_PER_REG                      (2)


#define NLM_MAX_NUM_DEVICE_ADDR                         (0x800000)

#define NLM_ADDR_OP_TYPE_BIT_START_POS                  (25)

#define NLM_NUM_ADDR_TYPES                              (4)

typedef enum NlmAddrType {
    NLMDEV_ADDR_TYPE_REG = 0,
    NLMDEV_ADDR_TYPE_DBA = 1,
    NLMDEV_ADDR_TYPE_UDA = 2
} NlmAddrType;

typedef enum NlmBankNum {
    NLMDEV_BANK_0 = 0,
    NLMDEV_BANK_1
} NlmBankNum;

typedef enum NlmPortNum {
    NLMDEV_PORT_0 = 0,
    NLMDEV_PORT_1
} NlmPortNum;

typedef enum NlmSMTNum {
    NLMDEV_SMT_0 = 0,
    NLMDEV_SMT_1
} NlmSMTNum;

typedef enum NlmPortMode {
    NLMDEV_SINGLE_PORT = 0,
    NLMDEV_DUAL_PORT
} NlmPortMode;

typedef enum NlmSMTMode {
    NLMDEV_NO_SMT_MODE = 0,
    NLMDEV_DUAL_SMT_MODE
} NlmSMTMode;

typedef enum NlmBlkCountDir {
    NLMDEV_BLK_COUNT_UP = 0,
    NLMDEV_BLK_COUNT_DOWN
} NlmBlkCountDir;

typedef enum NlmBlkInstType {
    NLMDEV_BLK_COPY_INST = 0,
    NLMDEV_BLK_MOVE_INST,
    NLMDEV_BLK_CLEAR_INST,
    NLMDEV_BLK_ENTRY_VALIDATE_INST
} NlmBlkInstType;


typedef enum NlmDevOperationMode_t {
    NLMDEV_OPR_STANDARD,
    NLMDEV_OPR_SAHASRA
} NlmDevOperationMode;

typedef enum NlmEMWriteMode {
    NLM_EM_WRITE_MODE_A,        
    NLM_EM_WRITE_MODE_B         
} NlmEMWriteMode;

typedef enum NlmEMReadMode {
    NLM_EM_READ_MODE_A,         
    NLM_EM_READ_MODE_B          
} NlmEMReadMode;

#define NLMDEV_AB_ADDR_LEN_IN_BYTES                     (4)
#define NLMDEV_REG_ADDR_LEN_IN_BYTES                    (4)
#define NLMDEV_AB_ENTRY_VALID_BIT_IN_ADDR               (30)
#define NLMDEV_AB_ENTRY_WR_MODE_BIT_IN_ADDR             (31)
#define NLMDEV_ADDR_TYPE_BIT_IN_PIO_WRITE               (25)    


#define NLMDEV_EM_ADDR_LEN_IN_BYTES                     (4)
#define NLMDEV_EM_ENTRY_VALID_BIT_IN_ADDR           (30)
#define NLMDEV_ADDR_TYPE_BIT_IN_EM_WRITE        (25)


#define NLMDEV_MAX_RESP_LEN_IN_BITS                     (640)
#define NLMDEV_MAX_RESP_LEN_IN_BYTES                    (NLMDEV_MAX_RESP_LEN_IN_BITS / 8)


#define NLMDEV_MAX_AD_LEN_IN_BITS                       (256)
#define NLMDEV_MAX_AD_LEN_IN_BYTES                      (NLMDEV_MAX_AD_LEN_IN_BITS / 8)


#define NLMDEV_CMP_RESULT_RV_BIT_POS                    (31)
#define NLMDEV_CMP_RESULT_SMF_BIT_POS                   (30)
#define NLMDEV_CMP_RESULT_RESP_FORMAT_END_BIT           (29)
#define NLMDEV_CMP_RESULT_RESP_FORMAT_START_BIT         (27)
#define NLMDEV_CMP_RESULT_DEVID_END_BIT                 (22)
#define NLMDEV_CMP_RESULT_DEVID_START_BIT               (21)
#define NLMDEV_CMP_LPM_RESULT_INDEX_END_BIT             (23)
#define NLMDEV_CMP_LPM_RESULT_INDEX_START_BIT           (0)
#define NLMDEV_CMP_RESULT_INDEX_END_BIT                 (20)
#define NLMDEV_CMP_RESULT_INDEX_START_BIT               (0)


#define NLM_REG_ADDR_DEVICE_ID                          (0x00000000)
#define NLM_REG_ADDR_DEVICE_CONFIG                      (0x00000001)
#define NLM_REG_ADDR_DEVICE_STATUS                      (0x00000002)
#define NLM_REG_ADDR_SOFT_SCAN_WRITE                    (0x00000004)
#define NLM_REG_ADDR_SOFT_ERROR_FIFO                    (0x00000005)

#define NLM_REG_ADDR_LPT_ENABLE                         (0x00000101)
#define NLM_REG_ADDR_SCRATCH_PAD0                       (0x00000102)
#define NLM_REG_ADDR_SCRATCH_PAD1                       (0x00000103)
#define NLM_RESULT_REG_ADDRESS_START                    (0x00000104)
#define NLM_RESULT_REG_ADDRESS_END                      (0x00000106)
#define NLM_REG_ADDR_UDA_SOFT_ERROR_COUNT               (0x00000107)
#define NLM_REG_ADDR_UDA_SOFT_ERROR_FIFO                (0x00000108)
#define NLM_REG_ADDR_UDA_CONFIG                         (0x0000010B)

#define NLM_REG_ADDR_ERROR_STATUS_PORT_0                (0x00000180)
#define NLM_REG_ADDR_ERROR_STATUS_PORT_1                (0x00000182)
#define NLM_REG_ADDR_INTERRUPT_MASK_PORT_0              (0x00000181)
#define NLM_REG_ADDR_INTERRUPT_MASK_PORT_1              (0x00000183)


#define NLM_REG_ADDR_BLK_CONFIG(x)                  (0x00001000 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR0_79_0(x)                   (0x00001001 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR0_159_80(x)                 (0x00001002 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR0_239_160(x)                (0x00001003 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR0_319_240(x)                (0x00001004 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR1_79_0(x)                   (0x00001005 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR1_159_80(x)                 (0x00001006 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR1_239_160(x)                (0x00001007 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR1_319_240(x)                (0x00001008 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR2_79_0(x)                   (0x00001009 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR2_159_80(x)                 (0x0000100A + ((x) * 0x20))
#define NLM_REG_ADDR_BMR2_239_160(x)                (0x0000100B + ((x) * 0x20))
#define NLM_REG_ADDR_BMR2_319_240(x)                (0x0000100C + ((x) * 0x20))
#define NLM_REG_ADDR_BMR3_79_0(x)                   (0x0000100D + ((x) * 0x20))
#define NLM_REG_ADDR_BMR3_159_80(x)                 (0x0000100E + ((x) * 0x20))
#define NLM_REG_ADDR_BMR3_239_160(x)                (0x0000100F + ((x) * 0x20))
#define NLM_REG_ADDR_BMR3_319_240(x)                (0x00001010 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR4_79_0(x)                   (0x00001011 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR4_159_80(x)                 (0x00001012 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR4_239_160(x)                (0x00001013 + ((x) * 0x20))
#define NLM_REG_ADDR_BMR4_319_240(x)                (0x00001014 + ((x) * 0x20))
#define NLM_NO_MASK_BMR_NUM                         (7)

#define NLM_BLK_REGISTERS_START_ADDR                NLM_REG_ADDR_BLK_CONFIG(0)
#define NLM_BLK_REGISTERS_END_ADDR                  NLM_REG_ADDR_BMR4_319_240(NLMDEV_NUM_ARRAY_BLOCKS-1)


#define NLM_REG_ADDR_LTR_BLOCK_SELECT0(x)           (0x00040000 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT1(x)           (0x00040001 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT2(x)           (0x00040002 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT3(x)           (0x00040003 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT4(x)           (0x00040004 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT5(x)           (0x00040005 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT6(x)           (0x00040006 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_BLOCK_SELECT7(x)           (0x00040007 + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH0(x)        (0x00040010 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH1(x)        (0x00040011 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH2(x)        (0x00040012 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH3(x)        (0x00040013 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH4(x)        (0x00040014 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH5(x)        (0x00040015 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH6(x)        (0x00040016 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH7(x)        (0x00040017 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH8(x)        (0x00040018 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH9(x)        (0x00040019 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH10(x)       (0x0004001A + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH11(x)       (0x0004001B + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH12(x)       (0x0004001C + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH13(x)       (0x0004001D + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH14(x)       (0x0004001E + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_PARALLEL_SEARCH15(x)       (0x0004001F + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_SB_KPU_SELECT(x)           (0x00040050 + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_RANGE_INSERTION0(x)        (0x00040058 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_RANGE_INSERTION1(x)        (0x00040059 + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_EXTENDED0(x)               (0x0004005A + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_KPU0_KEY_CONSTRUCTION0(x)  (0x0004005C + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU0_KEY_CONSTRUCTION1(x)  (0x0004005D + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU1_KEY_CONSTRUCTION0(x)  (0x0004005E + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU1_KEY_CONSTRUCTION1(x)  (0x0004005F + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU2_KEY_CONSTRUCTION0(x)  (0x00040060 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU2_KEY_CONSTRUCTION1(x)  (0x00040061 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU3_KEY_CONSTRUCTION0(x)  (0x00040062 + ((x) * 0x100))
#define NLM_REG_ADDR_LTR_KPU3_KEY_CONSTRUCTION1(x)  (0x00040063 + ((x) * 0x100))

#define NLM_REG_ADDR_LTR_OP_CODE_EXT(x)             (0x00040070 + ((x) * 0x100))

#define NLM_LTR_REG_GROUP_ADDRESS_START             (0x00040000)
#define NLM_LTR_REG_GROUP_ADDRESS_END               (NLM_REG_ADDR_LTR_OP_CODE_EXT(NLMDEV_MAX_NUM_LTRS-1))
#define NLM_LTR_NUM_OF_KCR_PER_KPU                  (2)
#define NLM_LTR_NUM_OF_SEGMENT_PER_KCR              (5)

#define NLMDEV_MAX_KEY_LEN_IN_BITS                  (640)
#define NLMDEV_MAX_KEY_LEN_IN_BYTES                 (NLMDEV_MAX_KEY_LEN_IN_BITS / 8)

#define NLM_NUM_BLKS_PER_BLOCK_SELECT_REG           (64)
#define NLM_NUM_SB_PER_BLOCK_SELECT_REG             (NLM_NUM_BLKS_PER_BLOCK_SELECT_REG/NLMDEV_NUM_AB_PER_SUPER_BLOCK)
#define NLM_NUM_BLKS_PER_PS_REG                     (32)


#define NLMDEV_MC_CB_REG_ADDRESS_RANGE_START        (0x00020000)
#define NLMDEV_MC_CB_REG_ADDRESS_RANGE_END          (0x00027FFF)
#define NLM_REG_ADDR_CONTEXT_BUFFER(x)              (0x00020000 + (x))


#define NLM_DEVICE_CONFIG_REG_PORT1_CTXID_SHIFT_END_BIT     (50)
#define NLM_DEVICE_CONFIG_REG_PORT1_CTXID_SHIFT_START_BIT   (48)
#define NLM_DEVICE_CONFIG_REG_AC_TO_BANK_MAPPING_END_BIT    (47)
#define NLM_DEVICE_CONFIG_REG_AC_TO_BANK_MAPPING_START_BIT  (32)
#define NLM_DEVICE_CONFIG_REG_SUPRESS_INDEX_ENABLE_BIT      (30)
#define NLM_DEVICE_CONFIG_REG_RANGE_ENABLE_BIT              (28)
#define NLM_DEVICE_CONFIG_REG_NW_BYTE_ORDER_ENABLE_BIT      (27)
#define NLM_DEVICE_CONFIG_REG_PORT1_ENABLE_BIT              (26)
#define NLM_DEVICE_CONFIG_REG_PORT0_ENABLE_BIT              (25)
#define NLM_DEVICE_CONFIG_REG_DUAL_PORT_MODE_BIT            (24)
#define NLM_DEVICE_CONFIG_REG_DUAL_BANK_MODE_BIT            (23)
#define NLM_DEVICE_CONFIG_REG_CB_CONFIG_END_BIT             (22)
#define NLM_DEVICE_CONFIG_REG_CB_CONFIG_START_BIT           (15)
#define NLM_DEVICE_CONFIG_REG_DB_PEEI_BIT                   (7)
#define NLM_DEVICE_CONFIG_REG_SOFT_ES_ENABLE_BIT            (6)
#define NLM_DEVICE_CONFIG_REG_PORT0_ERR_INTR_ON_PORT1       (5)
#define NLM_DEVICE_CONFIG_REG_PORT1_ERR_INTR_ON_PORT0       (4)
#define NLM_DEVICE_CONFIG_REG_MC_SEGMT_CAS_ENBL_BIT         (3)
#define NLM_DEVICE_CONFIG_REG_LAST_DEVICE_BIT               (1)
#define NLM_DEVICE_CONFIG_REG_FIRST_DEVICE_BIT              (0)


#define NLM_UDA_CONFIG_REG_uSBENABLE_END_BIT                (15)
#define NLM_UDA_CONFIG_REG_uSBENABLE_START_BIT              (0)


#define NLM_DEV_STATUS_REG_uSBENABLE_END_BIT                (15)
#define NLM_DEV_STATUS_REG_uSBENABLE_START_BIT              (0)


#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_INTVL_VAL_HI_END_BIT         (59)
#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_INTVL_VAL_HI_START_BIT       (44)
#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_INTVL_VAL_LO_END_BIT         (43)
#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_INTVL_VAL_LO_START_BIT       (12)
#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_ADDR_END_BIT                 (11)
#define NLM_DEV_DB_SOFT_ERR_SCAN_WRITE_ADDR_START_BIT               (0)


#define NLM_BLOCK_CONFIG_REG_ENABLE_BIT_POS         (0)
#define NLM_BLOCK_CONFIG_REG_WIDTH_START_BIT        (1)
#define NLM_BLOCK_CONFIG_REG_WIDTH_END_BIT          (2)
#define NLM_BLOCK_CONFIG_REG_SHIFT_VAL_START_BIT    (4)
#define NLM_BLOCK_CONFIG_REG_SHIFT_VAL_END_BIT      (5)
#define NLM_BLOCK_CONFIG_REG_BASE_ADDR_START_BIT    (6)
#define NLM_BLOCK_CONFIG_REG_BASE_ADDR_END_BIT      (19)
#define NLM_BLOCK_CONFIG_REG_SHIFT_DIR_BIT_POS      (24)


#define NLM_SB_CONFIG_START_BIT(x)          (2*x)
#define NLM_SB_CONFIG_END_BIT(x)            ((2*x) + 1)


#define NLM_PS_CONFIG_BLK_START_BIT(x)      (2*x)
#define NLM_PS_CONFIG_BLK_END_BIT(x)        ((2*x) + 1)

                                    
#define NLM_EXTENDED_REG_BMR_SELECT_START_BIT(x)        ((3*x))
#define NLM_EXTENDED_REG_BMR_SELECT_END_BIT(x)          ((3*x) + 2)

#define NLM_EXTENDED_REG_RANGE_A_EXTRACTION_START_BIT   (24)
#define NLM_EXTENDED_REG_RANGE_A_EXTRACTION_END_BIT     (30)
#define NLM_EXTENDED_REG_RANGE_B_EXTRACTION_START_BIT   (31)
#define NLM_EXTENDED_REG_RANGE_B_EXTRACTION_END_BIT     (37)
#define NLM_EXTENDED_REG_RANGE_C_EXTRACTION_START_BIT   (38)
#define NLM_EXTENDED_REG_RANGE_C_EXTRACTION_END_BIT     (44)
#define NLM_EXTENDED_REG_RANGE_D_EXTRACTION_START_BIT   (45)
#define NLM_EXTENDED_REG_RANGE_D_EXTRACTION_END_BIT     (51)

#define NLM_EXTENDED_REG_NUM_RESULTS_START_BIT          (56)
#define NLM_EXTENDED_REG_NUM_RESULTS_END_BIT            (57)
#define NLM_EXTENDED_REG_AD_RESOLUTION_BIT              (58)


#define NLM_OP_CODE_EXT_REG_LCL_OP_CODE_START_BIT   (0)
#define NLM_OP_CODE_EXT_REG_LCL_OP_CODE_END_BIT     (7)
#define NLM_OP_CODE_EXT_REG_RETURN_AD_BIT_POS(x)    (8 + (4*x))
#define NLM_OP_CODE_EXT_REG_AD_SIZE_START_BIT(x)    (9 + (4*x))
#define NLM_OP_CODE_EXT_REG_AD_SIZE_END_BIT(x)      (10 + (4*x))


#define NLM_KPU_KCR_NUM_BITS_FOR_START_LOC          (7)
#define NLM_KPU_KCR_NUM_BITS_FOR_NUM_BYTES          (4)
#define NLM_KPU_KCR_NUM_BITS_FOR_ZERO_FILL          (1)
#define NLM_KPU_KCR_NUM_BITS_PER_SEGMENT            (12)


#define NLM_KCR_START_BIT_FOR_START_LOC_SEG(x)  ((NLM_KPU_KCR_NUM_BITS_PER_SEGMENT*x))
#define NLM_KCR_END_BIT_FOR_START_LOC_SEG(x)     ((NLM_KPU_KCR_NUM_BITS_PER_SEGMENT*x) + 6)
#define NLM_KCR_START_BIT_FOR_NUM_BYTES_SEG(x)   ((NLM_KPU_KCR_NUM_BITS_PER_SEGMENT*x) + 7)
#define NLM_KCR_END_BIT_FOR_NUM_BYTES_SEG(x)     ((NLM_KPU_KCR_NUM_BITS_PER_SEGMENT*x) + 10)
#define NLM_KCR_BIT_FOR_ZERO_FILL(x)            ((NLM_KPU_KCR_NUM_BITS_PER_SEGMENT*x) + 11)


#define NLM_RESULT_REG_MATCH_ADDRESS_START_BIT(x)   ((40*x)+ 0)
#define NLM_RESULT_REG_MATCH_ADDRESS_END_BIT(x)     ((40*x)+ 23)
#define NLM_RESULT_REG_MATCH_FLAG_BIT_POS(x)        ((40*x)+ 39)


#define NLM_OPCODE_NOP_BITS_8_6             (0x0)
#define NLM_OPCODE_NOP_BITS_5_0             (0x0)

#define NLM_OPCODE_REG_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_REG_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_DBA_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_DBA_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_UDA_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_UDA_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_REG_READ_BITS_8_6        (0x0)
#define NLM_OPCODE_REG_READ_BITS_5_0        (0x2)

#define NLM_OPCODE_UDA_READ_BITS_8_6        (0x0)
#define NLM_OPCODE_UDA_READ_BITS_5_0        (0x2)

#define NLM_OPCODE_DBA_READ_X_BITS_8_6      (0x0)
#define NLM_OPCODE_DBA_READ_X_BITS_5_0      (0x2)

#define NLM_OPCODE_DBA_READ_Y_BITS_8_6      (0x0)
#define NLM_OPCODE_DBA_READ_Y_BITS_5_0      (0x3)

#define NLM_OPCODE_EM_WRA_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_WRA_BITS_5_0          (0x4)

#define NLM_OPCODE_EM_WRB_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_WRB_BITS_5_0          (0x5)

#define NLM_OPCODE_EM_RDA_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_RDA_BITS_5_0          (0x6)

#define NLM_OPCODE_EM_RDB_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_RDB_BITS_5_0          (0x7)

#define NLM_OPCODE_BLOCK_COPY_BITS_8_6      (0x0)
#define NLM_OPCODE_BLOCK_COPY_BITS_5_0      (0x8)

#define NLM_OPCODE_BLOCK_MOVE_BITS_8_6      (0x0)
#define NLM_OPCODE_BLOCK_MOVE_BITS_5_0      (0x9)

#define NLM_OPCODE_BLOCK_CLEAR_BITS_8_6     (0x0)
#define NLM_OPCODE_BLOCK_CLEAR_BITS_5_0     (0xA)

#define NLM_OPCODE_BLOCK_EV_BITS_8_6        (0x0)
#define NLM_OPCODE_BLOCK_EV_BITS_5_0        (0xB)

#define NLM_OPCODE_LPMTYPE1_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE1_BITS_5_0            (0xC)

#define NLM_OPCODE_LPMTYPE2_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE2_BITS_5_0            (0xD)

#define NLM_OPCODE_LPMTYPE3_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE3_BITS_5_0            (0xE)

#define NLM_OPCODE_CBWRITE_CMP1_BITS_8_6        (0x1)

#define NLM_OPCODE_CBWRITE_CMP2_BITS_8_6        (0x2)

#define NLM_OPCODE_CBWRITE_CMP3_BITS_8_6        (0x3)

#define NLM_OPCODE_CBWRITE_BITS_8_6             (0x4)

#define NLM_OPCODE_CBWLPM_BITS_8_6              (0x5)

#define NLM_OPCODE_PVT_UDA_WRITE                (0xF0)
#define NLM_OPCODE_PVT_UDA_READ                 (0xF1)
#define NLM_OPCODE_PVT_UDA_CONFIG               (0xF2)


#define NLM_OPCODE_PVT_MDIO_WRITE               (0xF3)
#define NLM_OPCODE_PVT_MDIO_READ                (0xF4)


#define NLM_OPCODE_PVT_LUT_WRITE                (0xF5)
#define NLM_OPCODE_PVT_LUT_READ                 (0xF6)


#define NLM_DATA_WORD_LEN_IN_BYTES              (8)
#define NLM_MAX_DATA_WORDS                      (5)
#define NLM_LPMTYPE1_NUM_DATA_WORDS             (5)
#define NLM_LPMTYPE2_NUM_DATA_WORDS             (2)
#define NLM_LPMTYPE3_NUM_DATA_WORDS             (2)

#define NLM_UDA_NUM_ROWS_SHIFT_VALUE            (13)


#define NLM_AC_2_BANK_MAPPING                   (0xFF00)
#define NLM_P1CTXID_SHIFT_VALUE                 (0x4)


#define NLMDEV_RANGE_DO_NOT_INSERT              (0x7F)
#define NLMDEV_RANGE_NUM_RANGE_TYPES            (4)
#define NLMDEV_RANGE_NUM_MCOR_PER_RANGE_TYPE    (8)
#define NLMDEV_RANGE_NUM_RANGE_CODE_REGS        (8)
#define NLMDEV_NUM_RANGE_REG                    (40)


#define NLM_REG_RANGE_A_BOUNDS(x)               (0x00085000 + ((x) * 0x01))
#define NLM_REG_RANGE_B_BOUNDS(x)               (0x00085008 + ((x) * 0x01))
#define NLM_REG_RANGE_C_BOUNDS(x)               (0x00085010 + ((x) * 0x01))
#define NLM_REG_RANGE_D_BOUNDS(x)               (0x00085018 + ((x) * 0x01))

#define NLM_REG_RANGE_A_CODE0                   (0x00085020)
#define NLM_REG_RANGE_A_CODE1                   (0x00085021)
#define NLM_REG_RANGE_B_CODE0                   (0x00085022)
#define NLM_REG_RANGE_B_CODE1                   (0x00085023)
#define NLM_REG_RANGE_C_CODE0                   (0x00085024)
#define NLM_REG_RANGE_C_CODE1                   (0x00085025)
#define NLM_REG_RANGE_D_CODE0                   (0x00085026)
#define NLM_REG_RANGE_D_CODE1                   (0x00085027)


#define NLMDEV_LUT_DATA_SIZE_START_BIT         (0)
#define NLMDEV_LUT_DATA_SIZE_END_BIT           (6)

#define NLMDEV_LUT_RECORD_TYPE_BIT             (7)
#define NLMDEV_LUT_RECORD_VALID_BIT            (8)

#define NLMDEV_LUT_MODE_START_BIT              (9)
#define NLMDEV_LUT_MODE_END_BIT               (11)

#define NLMDEV_LUT_KEY_CONFIG_BIT             (12)

#define NLMDEV_LUT_KEY_DATA_START_BIT         (13)
#define NLMDEV_LUT_KEY_DATA_END_BIT           (20)

#define NLMDEV_LUT_INSTRUCTION_START_BIT      (21)    
#define NLMDEV_LUT_INSTRUCTION_END_BIT        (30)

#define NLMDEV_LUT_KEY_WITH_COPY_80B_BIT      (31)

#define NLMDEV_LUT_COPY_DATA_CFG_START_BIT    (32)
#define NLMDEV_LUT_COPY_DATA_CFG_END_BIT      (35)

#define NLMDEV_LUT_IDX_OR_AD_NUM_BYTE_START_BIT(x)  (36 + (5 * x) )     
#define NLMDEV_LUT_IDX_OR_AD_NUM_BYTE_END_BIT(x)    (39 + (5 * x) )

#define NLMDEV_LUT_IDX_OR_AD_BIT(x)                 (40 + (5 * x) ) 

#endif
