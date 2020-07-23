/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:     ft_alu.h
 * Purpose:  Function declarations for flowtracker Alu.
 */

#ifndef _BCM_INT_FT_ALU_H_
#define _BCM_INT_FT_ALU_H_

#include <shared/bsl.h>
#include <soc/helix5.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_common.h>
#include <bcm_int/esw/flowtracker/ft_field.h>

#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

/*************************************************************
 | Start - Basic ALU/LOAD HW structs & defines               |
 */

typedef enum bcmi_ft_alu_load_type_e {

    /* Load8 resource type. */
    bcmiFtAluLoadTypeLoad8 = 0,

    /* Load16 resource type. */
    bcmiFtAluLoadTypeLoad16 = 1,

    /* Alu16 resource type. */
    bcmiFtAluLoadTypeAlu16 = 2,

    /* Alu32 resource type. */
    bcmiFtAluLoadTypeAlu32 = 3,

    /* Aggregate Alu32 resource type. */
    bcmiFtAluLoadTypeAggAlu32 = 4,

    /* Resource type none. */
    bcmiFtAluLoadTypeNone = 5

} bcmi_ft_alu_load_type_t;

extern uint8
    alu_type_str[6][50];

typedef struct bcmi_ft_alu_key_s {

    /* Location of tracking param. */
    int location;

    /* Is it a alu. */
    int is_alu;

    /* Length of tracking param. */
    int length;

} bcmi_ft_alu_key_t;

typedef enum bcmi_flowtracker_check_set_e {

    /* Check timestamp set. */
    bcmFlowtrackerFlowCheckTsSet = 0,

    /* Check timestamp reset. */
    bcmFlowtrackerFlowCheckTsReset = 1

} bcmi_flowtracker_check_set_t;

typedef struct bcmi_ft_flowchecker_list_s {

    /* Flowchecker identifier. */
    bcm_flowtracker_check_t flowchecker_id;

    /* Next pointer to checker identifier. */
    struct bcmi_ft_flowchecker_list_s *next;

} bcmi_ft_flowchecker_list_t;

/* ALU/LOAD internal flags. */
/* Next element is attached to this. */
#define BCMI_FT_ALU_LOAD_NEXT_ATTACH                      (1 << 0)

/* Aggregate ALU32 supported in the group */
#define BCMI_FT_ALU_LOAD_USE_AGG_ALU32                    (1 << 1)

/* Shift is applied to data (flowcheck action) */
#define BCMI_FT_ALU_LOAD_DATA_SHIFT_APPLIED               (1 << 2)

/* alu info is installed in the hardware */
#define BCMI_FT_ALU_LOAD_HW_INSTALLED                     (1 << 3)

/* Internally defined */
#define BCMI_FT_ALU_LOAD_INTERNAL                         (1 << 4)

/* Extended to other alu info */
#define BCMI_FT_ALU_LOAD_EXTENDED                         (1 << 5)

/* StickyOr update should be activated */
#define BCMI_FT_ALU_LOAD_STICKYOR_UPDATE                  (1 << 6)

/* Internal Flowchecker shift adjusted */
#define BCMI_FT_ALU_LOAD_INTERNAL_DATA_SHIFT_ADJUSTED     (1 << 7)
/*************************************************************
 | End - Basic ALU/LOAD HW structs & defines                 |
 */



/*************************************************************
 | Start - FT resource tracking structs & defines            |
 */

/* Session Data size in bits */
#define BCMI_FT_SESSION_DATA_SINGLE_LEN            256
#define BCMI_FT_SESSION_DATA_SINGLE_BANK_SIZE       64

/* Session Data Bank Mem and their data sizes. */
#define TOTAL_GROUP_SESSION_DATA_BANK_MEM            1
#define TOTAL_GROUP_SESSION_DATA_BANK_SW_DATA_NUM    4
#define TOTAL_GROUP_SESSION_DATA_BANK_DW_DATA_NUM    8
#define TOTAL_GROUP_AGG_SESSION_DATA_BANK_MEM        4

/* Total ALU memories and their data sizes. */
#define TOTAL_GROUP_ALU32_MEM                       12
#define TOTAL_GROUP_ALU32_DATA_NUM                   1
#define TOTAL_GROUP_ALU16_MEM                        8
#define TOTAL_GROUP_ALU16_DATA_NUM                   1
#define TOTAL_GROUP_LOAD16_MEM                       1
#define TOTAL_GROUP_LOAD16_DATA_NUM                 16
#define TOTAL_GROUP_LOAD8_MEM                        1
#define TOTAL_GROUP_LOAD8_DATA_NUM                   8
#define TOTAL_GROUP_TIMESTAMP_ENGINE_MEM             1
#define TOTAL_GROUP_TIMESTAMP_ENGINE_DATA_NUM        4
#define TOTAL_GROUP_AGG_ALU32_MEM                   16

/* ALU16/32 & LOAD8/16 */
#define BCMI_FT_ALU_LOAD_MEMS       (bcmiFtAluLoadTypeNone)

typedef struct alu_load_info_s {

    /* Memory identifier. */
    soc_mem_t mem;

    /* Memory size. */
    int mem_size;

    /* Flowtracker type. */
    bcmi_ft_type_t *resource_ft_type;

} alu_load_info_t;

typedef struct bcmi_ft_group_pdd_bus_info_s {

    /* Param identifier. */
    int param_id;

    /* End bit. */
    int end;

    /* Start bit. */
    int start;

    /* Length. */
    int length;

} bcmi_ft_group_pdd_bus_info_t;

typedef struct alu_load_index_info_s {

    /* Total memory count. */
    int total_mem_count;

    /* Total index per memory. */
    int total_idx_per_memory;

    /* Index Bitmap. */
    SHR_BITDCL *index_bitmap;

    /* Refcounts for all the indexes on this memory. */
    uint32 *idx_refcount;

    /* ALU/LOAD memory information. */
    alu_load_info_t *alu_load_info;

    /* PDD bus information. */
    bcmi_ft_group_pdd_bus_info_t *pdd_bus_info;

    /* Group field information. */
    uint32 *group_fid_info;

} alu_load_index_info_t;

/* There are 4 different types of memories. */
extern alu_load_index_info_t
      alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];

#define BCMI_FT_ALU_LOAD_INFO(_u, type) \
           (alu_load_index_info[_u][type])

#define BCMI_FT_ALU_LOAD_MEM_COUNT(_u, type) \
           ((alu_load_index_info[_u][type]).total_mem_count)

#define BCMI_FT_ALU_LOAD_MEM_TOTAL_INDEX(_u, type) \
           ((alu_load_index_info[_u][type]).total_idx_per_memory)

#define BCMI_FT_ALU_LOAD_BITMAP_INFO(_u, type) \
           (alu_load_index_info[_u][type].index_bitmap)

#define BCMI_FT_ALU_LOAD_REFCOUNT_INFO(_u, type) \
           (alu_load_index_info[_u][type].idx_refcount)

#define BCMI_FT_ALU_LOAD_REFCOUNT_INCR(_u, type, idx) \
           ((alu_load_index_info[_u][type].idx_refcount[idx])++)

#define BCMI_FT_ALU_LOAD_REFCOUNT_DEC(_u, type, idx) \
           ((alu_load_index_info[_u][type].idx_refcount[idx])--)

#define BCMI_FT_ALU_LOAD_REFCOUNT(_u, type, idx) \
           (alu_load_index_info[_u][type].idx_refcount[idx])

#define BCMI_FT_ALU_LOAD_GROUP_FID_INFO(_u, type) \
           ((alu_load_index_info[_u][type]).group_fid_info)

#define BCMI_FT_ALU_LOAD_PDD_INFO(_u, type) \
           (alu_load_index_info[_u][type].pdd_bus_info)

#define BCMI_FT_ALU_LOAD_MEM_INFO(_u, type) \
           ((alu_load_index_info[_u][type]).alu_load_info)

#define BCMI_FT_ALU_LOAD_MEM(_u, type, mem_idx) \
           (((BCMI_FT_ALU_LOAD_MEM_INFO(_u, type))[mem_idx]).mem)

#define BCMI_FT_ALU_LOAD_MEM_SIZE(_u, type, mem_idx) \
           (((BCMI_FT_ALU_LOAD_MEM_INFO(_u, type))[mem_idx]).mem_size)

#define BCMI_FT_ALU_LOAD_MEM_FT_TYPE(_u, type, mem_idx, bank_idx) \
   (((BCMI_FT_ALU_LOAD_MEM_INFO(_u, type))[mem_idx]).resource_ft_type[bank_idx])

/* Session Data Bank Info */
extern
alu_load_info_t *session_data_bank_info[BCM_MAX_NUM_UNITS];
#define BCMI_FT_SESSION_DATA_BANK_INFO(_u) \
            (session_data_bank_info[_u])

#define BCMI_FT_SESSION_DATA_BANK_MEM(_u) \
            ((session_data_bank_info[_u])->mem)

#define BCMI_FT_SESSION_DATA_BANK_MEM_SIZE(_u) \
            ((session_data_bank_info[_u])->mem_size)

#define BCMI_FT_SESSION_DATA_BANK_FT_TYPE(_u, idx) \
            ((session_data_bank_info[_u])->resource_ft_type[idx])

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
extern
alu_load_info_t *agg_session_data_bank_info[BCM_MAX_NUM_UNITS];
#define BCMI_FT_AGG_SESSION_DATA_BANK_INFO(_u) \
            (agg_session_data_bank_info[_u])

#define BCMI_FT_AGG_SESSION_DATA_BANK_MEM(_u, _idx) \
            ((agg_session_data_bank_info[_u])[_idx].mem)

#define BCMI_FT_AGG_SESSION_DATA_BANK_MEM_SIZE(_u, _idx) \
            ((agg_session_data_bank_info[_u])[_idx].mem_size)

#define BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(_u, _idx, _ridx) \
            ((agg_session_data_bank_info[_u])[_idx].resource_ft_type[_ridx])
#endif

/* TimeStamp Engine Info */
extern
alu_load_info_t *timestamp_engine_info[BCM_MAX_NUM_UNITS];
#define BCMI_FT_TIMESTAMP_ENGINE_INFO(_u) \
            (timestamp_engine_info[_u])

#define BCMI_FT_TIMESTAMP_ENGINE_MEM(_u) \
            ((timestamp_engine_info[_u])->mem)

#define BCMI_FT_TIMESTAMP_ENGINE_MEM_SIZE(_u) \
            ((timestamp_engine_info[_u])->mem_size)

#define BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(_u, idx) \
            ((timestamp_engine_info[_u])->resource_ft_type[idx])

/* Resource Software State Info */
typedef struct bcmi_ft_resource_sw_info_s {

    /* Number of banks allocated to each FT stage. */
    uint8 num_banks[bcmiFtTypeCount];

    /* Local PDE start offset for each FT stage. */
    uint8 bank_start_local_pde_offset[bcmiFtTypeCount];

    /* Export PDE start offset for each FT stage. */
    uint8 bank_start_export_pde_offset[bcmiFtTypeCount];

    /* Local offset jump for each FT stage. */
    uint8 bank_offset_local_jump[bcmiFtTypeCount];

    /* Export offset jump for each FT stage. */
    uint8 bank_offset_export_jump[bcmiFtTypeCount];

    /* Fill bytes for each FT stage. */
    uint8 bank_fill_bytes[bcmiFtTypeCount];

    /* No. of fill for each FT stage. */
    uint8 bank_num_fills[bcmiFtTypeCount];

    /* No. of Load8 resource allocated to each FT stage. */
    uint8 num_load8[bcmiFtTypeCount];

    /* No. of Load16 resource allocated to each FT stage. */
    uint8 num_load16[bcmiFtTypeCount];

    /* No. of timestamp resource allocated to each FT stage. */
    uint8 num_tsbank[bcmiFtTypeCount];

    /* Chip delay granularity. */
    uint8 chip_delay_gran;

    /* End to End delay granularity. */
    uint8 e2e_delay_gran;

    /* Inter packet arrival delay granularity. */
    uint8 ipat_delay_gran;

    /* Inter packet departure delay granularity. */
    uint8 ipdt_delay_gran;

    /* Chip delay offset. */
    int chip_delay_offset;

    /* End to End delay offset. */
    int e2e_delay_offset;

    /* Inter packet arrival delay offset. */
    int ipat_delay_offset;

    /* Inter packet departure delay offset. */
    int ipdt_delay_offset;

} bcmi_ft_resource_sw_info_t;

extern bcmi_ft_resource_sw_info_t
        bcmi_ft_resource_sw_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_RESOURCE_SW_INFO(unit) \
        (bcmi_ft_resource_sw_state[unit])

#define BCMI_FT_RESOURCE_SW_INFO_NUM_BANKS(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].num_banks[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_START_LOCAL_PDE_OFFSET(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_start_local_pde_offset[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_START_EXPORT_PDE_OFFSET(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_start_export_pde_offset[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_LOCAL_JUMP(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_offset_local_jump[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_OFFSET_EXPORT_JUMP(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_offset_export_jump[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_FILL_BYTES(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_fill_bytes[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_FILLS(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].bank_num_fills[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD8(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].num_load8[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_LOAD16(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].num_load16[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_BANK_NUM_TS(unit, ft_type)  \
        (bcmi_ft_resource_sw_state[unit].num_tsbank[ft_type])

#define BCMI_FT_RESOURCE_SW_INFO_CHIP_GRAN(unit)  \
        (bcmi_ft_resource_sw_state[unit].chip_delay_gran)

#define BCMI_FT_RESOURCE_SW_INFO_E2E_GRAN(unit)  \
        (bcmi_ft_resource_sw_state[unit].e2e_delay_gran)

#define BCMI_FT_RESOURCE_SW_INFO_IPAT_GRAN(unit)  \
        (bcmi_ft_resource_sw_state[unit].ipat_delay_gran)

#define BCMI_FT_RESOURCE_SW_INFO_IPDT_GRAN(unit)  \
        (bcmi_ft_resource_sw_state[unit].ipdt_delay_gran)

#define BCMI_FT_RESOURCE_SW_INFO_CHIP_OFFSET(unit)  \
        (bcmi_ft_resource_sw_state[unit].chip_delay_offset)

#define BCMI_FT_RESOURCE_SW_INFO_E2E_OFFSET(unit)  \
        (bcmi_ft_resource_sw_state[unit].e2e_delay_offset)

#define BCMI_FT_RESOURCE_SW_INFO_IPAT_OFFSET(unit)  \
        (bcmi_ft_resource_sw_state[unit].ipat_delay_offset)

#define BCMI_FT_RESOURCE_SW_INFO_IPDT_OFFSET(unit)  \
        (bcmi_ft_resource_sw_state[unit].ipdt_delay_offset)
/*************************************************************
 | End - FT resource tracking structs & defines              |
 */


/*************************************************************
 | Start - ALU Hash structs & defines                        |
 */

/* Hash table defines. */
#define BCMI_FT_ALU_HASH_BUCKETS              1024
#define BCMI_FT_ALU_HASH_KEY_LEN_IN_WORDS     8

/* Below is the information what will be stored
 * in hash table. */
typedef struct bcmi_ft_alu_hash_info_s {

    /* ALU/LOAD index allocated. */
    int alu_load_index;

    /* ALU/LOAD type. */
    bcmi_ft_alu_load_type_t alu_load_type;

    /* ALU/LOAD key. */
    bcmi_ft_alu_key_t key;

    /* Tracking param. */
    uint16 param;

    /* Custom Id */
    uint16 custom_id;

    /* Id of this flowchecker. */
    bcmi_ft_flowchecker_list_t *head;

    /* Flowtracker type (None/IFT/MFT/EFT).
     * Not valid for LOAD8/16 memories. */
    bcmi_ft_type_t hash_ft_type;

    /* LOAD8/16 banks allocated. */
    uint32  hash_load_mem_vector;

    /* Group direction. */
    bcm_flowtracker_direction_t direction;

} bcmi_ft_alu_hash_info_t;

typedef struct bcmi_ft_alu_entry_list_s {

    /* ALU hash info. */
    bcmi_ft_alu_hash_info_t alu_info;

    /* Link to next hash entry. */
    struct bcmi_ft_alu_entry_list_s *link;

} bcmi_ft_alu_entry_list_t;

typedef struct bcmi_ft_alu_table_hash_s {

    /* Head alu hash entry link. */
    bcmi_ft_alu_entry_list_t *head_link;

} bcmi_ft_alu_table_hash_t;

/*************************************************************
 | End - ALU Hash structs & defines                          |
 */



/*************************************************************
 | Start - FT checker structs & defines                      |
 */

/* Flowtracker checker software info. */
typedef struct bcmi_flowtracker_flowchecker_info_s {

    /* Check flags. */
    uint32 flags;

    /* Primary check of this flowchecker. */
    bcm_flowtracker_check_info_t check1;

    /* Second check of this flowchecker. */
    bcm_flowtracker_check_info_t check2;

    /* Action to be performed for this flowchecker. */
    bcm_flowtracker_check_action_info_t action_info;

    /* Export action information for this flowchecker. */
    bcm_flowtracker_check_export_info_t export_info;

    /* Delay information for this flowchecker. */
    bcm_flowtracker_check_delay_info_t delay_info;

    /* Supported flowtracker type bitmap. */
    uint16 check_ft_type_bmp;

} bcmi_flowtracker_flowchecker_info_t;

/* Flowtracker checker software state. */
typedef struct bcmi_ft_flowchecker_sw_info_s {

    /* Flowchecker software info. */
    bcmi_flowtracker_flowchecker_info_t **bcmi_sw_info;

    /* Flowchecker index bitmap. */
    SHR_BITDCL *ft_flowchecker_bitmap;

    /* Flowchecker index reference count. */
    uint32 *fc_idx_refcount;

    /* Flowchecker maximum id. */
    uint16 max_check;

} bcmi_ft_flowchecker_sw_state_t;

extern bcmi_ft_flowchecker_sw_state_t
           bcmi_ft_flowchecker_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_FLOWCHECKER_BITMAP(unit)  \
            (bcmi_ft_flowchecker_state[unit].ft_flowchecker_bitmap)

#define BCMI_FT_FLOWCHECKER_STATE(unit)  \
            (bcmi_ft_flowchecker_state[unit].bcmi_sw_info)

#define BCMI_FT_FLOWCHECKER_INFO(unit, index)  \
            (bcmi_ft_flowchecker_state[unit].bcmi_sw_info[index])

#define BCMI_FT_FLOWCHECKER_REFCOUNT_STATE(unit)  \
            (bcmi_ft_flowchecker_state[unit].fc_idx_refcount)

#define BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)  \
            (bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])

#define BCMI_FT_FLOWCHECKER_REFCOUNT_DEC(unit, index)  \
            ((bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])--)

#define BCMI_FT_FLOWCHECKER_REFCOUNT_INC(unit, index)  \
            ((bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])++)

#define BCMI_FT_FLOWCHECKER_MAX_ID(unit)  \
            (bcmi_ft_flowchecker_state[unit].max_check)

/*************************************************************
 | End - FT checker structs & defines                        |
 */



/*************************************************************
 | Start - ALU/LOAD HW structs & defines                     |
 */

/* Below is the information that FTFP will send to Group. */
typedef struct bcmi_ft_group_alu_info_s {

    /* Flags. */
    uint32 flags;

    /* Check 1 data.*/
    bcmi_ft_alu_key_t key1;
    /* Element type. */
    bcm_flowtracker_tracking_param_type_t element_type1;

    /* Check 2 data.*/
    bcmi_ft_alu_key_t key2;
    /* Element type. */
    bcm_flowtracker_tracking_param_type_t element_type2;

    /* action data.*/
    bcmi_ft_alu_key_t action_key;

    /* Element type. */
    bcm_flowtracker_tracking_param_type_t action_element_type;

    /* id of this flowchecker */
    bcm_flowtracker_check_t flowchecker_id;

    /* Flowtracker type (None/IFT/MFT/EFT).
     * Not valid for LOAD8/16 memories. */
    bcmi_ft_type_t alu_ft_type;

    /* These are needed for warmboot and may be for SER. */
    int alu_load_index;

    /* ALU/LOAD type */
    bcmi_ft_alu_load_type_t alu_load_type;

    /* Flow direction */
    bcm_flowtracker_direction_t direction;

    /* Custom Ids for Key1 */
    uint16 custom_id1;

    /* Custom Ids for Key2 */
    uint16 custom_id2;

    /* Custom Ids for action_key */
    uint16 action_custom_id;

} bcmi_ft_group_alu_info_t;

typedef struct bcmi_ft_element_alu_info_s {

    /* Tracking param type. */
    bcm_flowtracker_tracking_param_type_t element;

    /* Flowchecker operation. */
    bcm_flowtracker_check_operation_t operation;

    /* Flowchecker action. */
    bcm_flowtracker_check_action_t action;

} bcmi_ft_element_alu_info_t;

/* Select packet attribute from Extracted bus for check */
#define BCMI_FT_ALU_ATTR_SELECT_EXTRACTION     0

/* Select byte count for check */
#define BCMI_FT_ALU_ATTR_SELECT_BYTE_COUNT     1

/* ALU Check logic Kmap bit selection */
/* DO NOT CHANGE. Hw encoding */
#define BCMI_FT_ALU_KMAP_BIT_SEL_CHECK0_RES     0
#define BCMI_FT_ALU_KMAP_BIT_SEL_CHECK1_RES     1
#define BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS0     2
#define BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS1     3
#define BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS2     4
#define BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS3     5
#define BCMI_FT_ALU_KMAP_BIT_SEL_PKT_FLAGS4     6
#define BCMI_FT_ALU_KMAP_BIT_SEL_STATE_TRIG     7

/* Vector definitions for various ALU/LOAD methods. */
typedef struct bcmi_ft_alu_load_method_s {

    int (*alu_load_match)(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *temp,
            bcmi_ft_alu_hash_info_t *alu_info,
            int *load16_indexes, int *index);

    int (*alu_load_install)(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes);

    int (*alu_load_uninstall)(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *info,
            int *indexes);

} bcmi_ft_alu_load_method_t;

extern
bcmi_ft_alu_load_method_t
    *alu_load_methods[BCMI_FT_ALU_LOAD_MEMS];

/* ALU/LOAD match methods. */
#define bcmi_ft_method_match(type) \
    (alu_load_methods[type]->alu_load_match)

#define BCMI_FT_METHOD_MATCH_CHECK(type) \
    do { \
        if (!bcmi_ft_method_match(type)) { \
            rv = BCM_E_UNAVAIL; \
        } \
    } while(0);

#define BCMI_FT_METHOD_MATCH_EXEC(type) \
    (*(bcmi_ft_method_match(type)))


/* ALU/LOAD Install methods. */
#define bcmi_ft_method_install(type) \
    (alu_load_methods[type]->alu_load_install)

#define BCMI_FT_METHOD_INSTALL_CHECK(type) \
    do { \
        if (!bcmi_ft_method_install(type)) { \
            rv = BCM_E_UNAVAIL; \
        } \
    } while(0);

#define BCMI_FT_METHOD_INSTALL_EXEC(type) \
    (*(bcmi_ft_method_install(type)))

/* ALU/LOAD uninstall methods. */
#define bcmi_ft_method_uninstall(type) \
    (alu_load_methods[type]->alu_load_uninstall)

#define BCMI_FT_METHOD_UNINSTALL_CHECK(type) \
    do { \
        if (!bcmi_ft_method_uninstall(type)) { \
            rv = BCM_E_UNAVAIL; \
        } \
    } while(0);

#define BCMI_FT_METHOD_UNINSTALL_EXEC(type) \
    (*(bcmi_ft_method_uninstall(type)))

/*************************************************************
 | End - ALU/LOAD HW structs & defines                       |
 */



/*************************************************************
 | Start - Chip debug structs & defines                      |
 */

typedef struct bcmi_ft_chip_debug_param_s {

    /* Tracking param type. */
    bcm_flowtracker_tracking_param_type_t param;

    /* ALU/LOAD type. */
    bcmi_ft_alu_load_type_t alu_type;

    /* Memory identifier. */
    int mem_id;

    /* Count. */
    uint32 count;

} bcmi_ft_chip_debug_param_t;

typedef struct bcmi_ft_chip_debug_s {

    /* Chip debug enable status. */
    int chip_debug_enable;

    /* Number of debug information. */
    int num_debug_info;

    /* ALU32 mem resource used. */
    int alu32_mem_used[TOTAL_GROUP_ALU32_MEM];

    /* ALU16 mem resource used. */
    int alu16_mem_used[TOTAL_GROUP_ALU16_MEM];

    /* ALU32 mem resource reference count. */
    int alu32_ref_count[TOTAL_GROUP_ALU32_MEM];

    /* ALU16 mem resource reference count. */
    int alu16_ref_count[TOTAL_GROUP_ALU16_MEM];

    /* Chip debug param information. */
    bcmi_ft_chip_debug_param_t *param_info;

} bcmi_ft_chip_debug_t;

extern
bcmi_ft_chip_debug_t chip_debug_info[BCM_MAX_NUM_UNITS];

#define BCMI_FT_CHIP_DEBUG_INFO(unit) \
    (chip_debug_info[unit])

#define BCMI_FT_CHIP_DEBUG_ENABLE(unit) \
    ((chip_debug_info[unit]).chip_debug_enable)

#define BCMI_FT_CHIP_NUM_DEBUG_INFO(unit) \
    ((chip_debug_info[unit]).num_debug_info)

#define BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) \
    (chip_debug_info[unit].param_info)

#define BCMI_FT_CHIP_DEBUG_PARAM(unit, id) \
    (chip_debug_info[unit].param_info[id])

/*************************************************************
 | End - Chip debug structs & defines                        |
 */




/*************************************************************
 | Start - FT resource methods                               |
 */

extern int
bcmi_ft_alu_load_init(int unit);

extern void
bcmi_ft_alu_load_cleanup(int unit);

/*************************************************************
 | End - FT resource methods                                 |
 */



/*************************************************************
 | Start - ALU Hash methods                                  |
 */

extern int
bcmi_ft_alu_hash_init(int unit);

extern void
bcmi_ft_alu_hash_cleanup(int unit);

/* Functions for hash insert/remove. */
extern int
bcmi_ft_alu_hash_insert(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_group_alu_info_t *group_alu_info,
        int *load_indexes,
        int *alu_load_index);

extern int
bcmi_ft_alu_hash_remove(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_group_alu_info_t *group_alu_info,
        int *load_indexes,
        int *alu_load_index);

extern int
bcmi_ft_alu_hash_mem_index_get (
        int unit,  bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_group_alu_info_t *group_alu_info,
        int *load_indexes,
        int *alu_load_index);

extern int
bcmi_ft_alu_load_compare(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_alu_hash_info_t *alu_info,
        int *indexes);

extern int
bcmi_ft_alu_check16_mapping_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_check_t *check_map);

extern int
bcmi_ft_alu_load_hash_dump(
        int unit);

/*************************************************************
 | End - ALU Hash methods                                    |
 */


/*************************************************************
 | Start - ALU hardware methods                              |
 */

extern int
bcmi_ft_alu_entry_match(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *temp,
        bcmi_ft_alu_hash_info_t *alu_info,
        int *load8_indexes, int *index);

extern int
bcmi_ft_alu32_entry_install(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_alu32_entry_uninstall(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_alu16_entry_install(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_alu16_entry_uninstall(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_load16_entry_match(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *temp,
        bcmi_ft_alu_hash_info_t *alu_info,
        int *load16_indexes, int *index);

extern int
bcmi_ft_load16_entry_install(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_load16_entry_uninstall(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_load8_entry_match(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *temp,
        bcmi_ft_alu_hash_info_t *alu_info,
        int *load8_indexes, int *index);

extern int
bcmi_ft_load8_entry_install(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

extern int
bcmi_ft_load8_entry_uninstall(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        int *indexes);

/* Utility Functions - install/uninstall. */
extern int
bcmi_ft_flowchecker_alu_base_control_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *info,
        uint32 *alu_fmt);

extern int
bcmi_ft_load16_entry_write(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *temp,
        int *load16_indexes, int index);

extern int
bcmi_ft_load8_entry_write(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *temp,
        int *load8_indexes, int index);

extern int
bcmi_ft_load16_entry_clear(int unit, int index);

/* Utility Functions - index. */
extern int
bcmi_ft_alu_check_free_index(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int index);

extern int
bcmi_ft_alu_get_free_index(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_group_alu_info_t *group_alu_info,
        int *load_indexes,
        int *index);

extern void
bcmi_ft_alu_load_mem_index_get(
        int unit,
        int index, bcmi_ft_alu_load_type_t type,
        int *new_index, soc_mem_t *mem, int *bit);

/* Utility methods - other. */
extern void
bcmi_ft_alu_load_pdd_mem_get(int unit,
        bcmi_ft_type_t ft_type,
        soc_mem_t *mem);

extern int
bcmi_ft_alu_hw_operation_get(
        int unit,
        bcm_flowtracker_check_operation_t operation,
        int *check_0_operation,
        int *check_1_operation);

extern int
bcmi_ft_flowchecker_opr_is_dual(
        int unit,
        bcm_flowtracker_check_operation_t operation);

extern int
bcmi_ft_alu_hw_action_get(
        int unit,
        bcmi_ft_group_alu_info_t *alu_info,
        bcm_flowtracker_check_action_t action,
        int * hw_action);

extern void
bcmi_ft_group_alu_action_shift_bits_get(
        int unit,
        bcm_flowtracker_check_action_t action,
        bcmi_ft_group_alu_info_t *info,
        int *action_shift_bits);

extern int
bcmi_ft_group_alu_load_trigger_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_direction_t direction);

extern int
bcmi_ft_alu_load_values_normalize(int unit,
                                  bcmi_ft_group_alu_info_t *info,
                                  int *length,
                                  int *alu);

extern int
bcmi_ft_alu_load_type_get(
        int unit,
        int alu,
        int length,
        bcmi_ft_alu_load_type_t *type);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
extern void
bcmi_ftv2_get_load_mem_vector(int unit,
                              bcm_flowtracker_group_t id,
                              bcmi_ft_alu_load_type_t alu_load_type,
                              int *load_indexes,
                              uint32 *load_mem_vector);
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

extern int
bcmi_ft_group_alu_load_index_match(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_alu_hash_info_t *alu_info);

extern int
bcmi_ft_tracking_param_in_Debug_mode(int unit,
                                     bcm_flowtracker_group_t id,
                                     bcmi_ft_group_alu_info_t *group_alu_info,
                                     bcmi_ft_alu_load_type_t alu_load_type);

extern int
bcmi_ft_chip_debug_counter_get(int unit,
                               int mem_idx,
                               bcmi_ft_alu_load_type_t alu_load_type,
                               uint32 *count);

extern void
bcmi_ft_chip_debug_use_mem_update(int unit,
                                  bcmi_ft_alu_load_type_t alu_load_type,
                                  int mem_idx,
                                  int val);

extern int
bcmi_ft_chip_debug_mem_check(int unit,
                             bcmi_ft_alu_load_type_t alu_load_type,
                             int mem_idx);

extern int
bcmi_ft_chip_debug_info_get(int unit,
                            int max_size,
                            bcm_flowtracker_chip_debug_info_t *info,
                            int *num_debug_info);

extern int
bcmi_ft_chip_debug_alu_index_get(int unit,
                                 bcm_flowtracker_group_t id,
                                 bcmi_ft_group_alu_info_t *group_alu_info,
                                 bcmi_ft_alu_load_type_t alu_load_type,
                                 int *index);

extern int
bcmi_ft_chip_debug_data_sanity(int unit,
                               int num_info,
                               bcm_flowtracker_chip_debug_info_t *info);

/* Get the matching data from valid pdd entry. */
extern int
bcmi_ft_group_alu_load_index_match_data_get(int unit,
                                            bcm_flowtracker_group_t id,
                                            bcmi_ft_alu_load_type_t alu_load_type,
                                            int iter,
                                            bcmi_ft_group_alu_info_t **temp);

extern int
bcmi_ft_group_alu_load_pdd_status_get(int unit,
                                      bcm_flowtracker_group_t id,
                                      uint32 *in_pdd_entry,
                                      bcmi_ft_alu_load_type_t alu_load_type,
                                      int iter);

extern int
bcmi_ft_tracking_param_weight_hw_val_get(
        int unit,
        bcm_flowtracker_tracking_param_weight_t weight,
        uint32 *hw_val);

extern int
bcmit_ft_tracking_param_weight_enum_get(
        int unit,
        uint32 hw_val,
        bcm_flowtracker_tracking_param_weight_t *weight);

extern int
bcmi_ft_check_ft_type_bmp_set(
        int unit,
        bcmi_flowtracker_flowchecker_info_t *fc_info);

/*************************************************************
 | End - ALU hardware methods                                |
 */



/*************************************************************
 | Start - Chip debug methods                                |
 */

extern int
bcmi_ft_chip_debug_info_set(int unit,
                            int num_info,
                            bcm_flowtracker_chip_debug_info_t *info);

extern int
bcmi_ft_chip_debug_info_get(int unit,
                            int max_size,
                            bcm_flowtracker_chip_debug_info_t *info,
                            int *num_debug_info);

/*************************************************************
 | End - Chip debug methods                                  |
 */



/*************************************************************
 | Start - Flowtracker version 2 methods                     |
 */

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
extern int
bcmi_ftv2_alu_load_mem_config_init(int unit);

extern int
bcmi_ftv2_resource_info_init(int unit);

extern int
bcmi_ftv2_group_alu_mask_set(int unit,
                             bcmi_ft_group_alu_info_t *alu_info,
                             bcmi_flowtracker_flowchecker_info_t *fc_info,
                             uint32 *alu_fmt);

extern int
bcmi_ftv2_group_alu16_mask_clear(int unit,
                                 soc_mem_t mem,
                                 int index);

extern int
bcmi_ftv2_group_alu32_mask_clear(int unit,
                                 bcmi_ft_alu_load_type_t alu_load_type,
                                 soc_mem_t mem,
                                 int index);

extern int
bcmi_ftv2_group_param_delay_set(int unit,
                                bcm_flowtracker_group_t id,
                                bcmi_ft_group_alu_info_t *alu_info,
                                uint32 *alu32_specific_ctrlfmt);

extern int
bcmi_ftv2_group_check_delay_set(int unit,
                                bcm_flowtracker_group_t id,
                                bcmi_flowtracker_flowchecker_info_t *fc_info,
                                uint32 *alu32_specific_ctrlfmt);
extern int
bcmi_ftv2_alu_hw_timestamp_get(int unit,
                               bcm_flowtracker_group_t id,
                               int user_req,
                               bcm_flowtracker_timestamp_source_t sw_ts,
                               int *hw_ts);
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*************************************************************
 | End - Flowtracker version 2 methods                       |
 */

/*************************************************************
 | Start - Flowtracker version 3 methods                     |
 */

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
extern int
bcmi_ftv3_alu_load_mem_config_init(int unit);

extern void
bcmi_ftv3_alu_check_attr_select_get(int unit,
        bcmi_flowtracker_flowchecker_info_t *fc_info,
        int *check1_attr_sel,
        int *check2_attr_sel,
        int *update_attr_sel);

#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

/*************************************************************
 | End - Flowtracker version 3 methods                       |
 */

#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_ALU_H_ */
