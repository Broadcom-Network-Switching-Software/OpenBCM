/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_group.h
 * Purpose:     Function declarations for flowtracker group.
 */

#ifndef _BCM_INT_FT_ALU_H_
#define _BCM_INT_FT_ALU_H_

#include <shared/bsl.h>
#include <sal/core/boot.h>
#include <soc/helix5.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/mspi.h>
#include <bcm_int/esw/trx.h>
#include <bcm/flowtracker.h>
#include <bcm_int/esw/flowtracker/ft_field.h>


#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)


#define BCMI_FT_SESSION_DATA_SINGLE_LEN    256


/************************************************************************/

/* Groups internal Declarations. */

/* These are 1-bit attributes of packet/objects of flow. */
typedef enum bcm_flowtracker_flowchecker_attr_e {
    bcmFlowtrackerFlowCheckerAttrTcpFlagUrg = 0,
    bcmFlowtrackerFlowCheckerAttrTcpFlagAck = 1,
    bcmFlowtrackerFlowCheckerAttrTcpFlagPsh = 2,
    bcmFlowtrackerFlowCheckerAttrTcpFlagRst = 3,
    bcmFlowtrackerFlowCheckerAttrTcpFlagSyn = 4,
    bcmFlowtrackerFlowCheckerAttrTcpFlagFin = 5,
    bcmFlowtrackerFlowCheckerAttrCount = 6
} bcm_flowtracker_flowchecker_attr_t;

typedef enum bcmi_flowtracker_check_set_e {
    bcmFlowtrackerFlowCheckTsSet=0,
    bcmFlowtrackerFlowCheckTsReset = 1
} bcmi_flowtracker_check_set_t;

typedef struct bcmi_flowtracker_flowchecker_info_s {

    uint32 flags;

    /* Primary check of this flowchecker. */
    bcm_flowtracker_check_info_t check1;

    /* Second check of this flowchecker. */
    bcm_flowtracker_check_info_t check2;

    /* Action to be performed for this flowchecker. */
    bcm_flowtracker_check_action_info_t action_info;

    /* Export action information for this flowchecker. */
    bcm_flowtracker_check_export_info_t export_info;

}  bcmi_flowtracker_flowchecker_info_t;



typedef struct bcmi_ft_flowchecker_list_s {
    bcm_flowtracker_check_t flowchecker_id;
    struct bcmi_ft_flowchecker_list_s *next;
} bcmi_ft_flowchecker_list_t;


typedef struct bcmi_ft_alu_key_s {
    int location;
    int is_alu;
    int length;
} bcmi_ft_alu_key_t;

typedef enum bcmi_ft_alu_load_type_e {
    bcmiFtAluLoadTypeLoad8=0,
    bcmiFtAluLoadTypeLoad16=1,
    bcmiFtAluLoadTypeAlu16=2,
    bcmiFtAluLoadTypeAlu32=3,
    bcmiFtAluLoadTypeNone=4
} bcmi_ft_alu_load_type_t;

extern uint8 alu_type_str[5][50];

/* ALU/LOAD internal flags. */
/* Next element is attached to this. */
#define BCMI_FT_ALU_LOAD_NEXT_ATTACH           (1 << 0)

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

    /* id of this flowchecker. */
    bcm_flowtracker_check_t flowchecker_id;

    /* These are needed for warmboot and may be for SER. */
    int alu_load_index;
    bcmi_ft_alu_load_type_t alu_load_type;

} bcmi_ft_group_alu_info_t;


typedef struct bcmi_ft_element_alu_info_s {
    bcm_flowtracker_tracking_param_type_t element;

    bcm_flowtracker_check_operation_t operation;
    bcm_flowtracker_check_action_t    action;
} bcmi_ft_element_alu_info_t;

/* Below is the information what will be stored in hash table. */
typedef struct bcmi_ft_alu_hash_info_s {
    int alu_load_index;
    bcmi_ft_alu_load_type_t alu_load_type;

    bcmi_ft_alu_key_t key;

    uint16 param;

    /* Update parameters. */
    int update_location;

    /* id of this flowchecker. */
    bcmi_ft_flowchecker_list_t *head;

    /* Group direction. */
    bcm_flowtracker_direction_t direction;

} bcmi_ft_alu_hash_info_t;



typedef struct bcmi_ft_alu_entry_list_s {
    bcmi_ft_alu_hash_info_t alu_info;
    struct bcmi_ft_alu_entry_list_s *link;
} bcmi_ft_alu_entry_list_t;

typedef struct bcmi_ft_alu_table_hash_s {
    bcmi_ft_alu_entry_list_t *head_link;
} bcmi_ft_alu_table_hash_t;

/* Hash table defines. */
#define BCMI_FT_ALU_HASH_BUCKETS              1024
#define BCMI_FT_ALU_HASH_KEY_LEN_IN_WORDS     7



/* Total ALU memories and their data sizes. */
#define TOTAL_GROUP_ALU32_MEM         12
#define TOTAL_GROUP_ALU16_MEM         8
#define TOTAL_GROUP_LOAD16_MEM        1
#define TOTAL_GROUP_LOAD8_MEM         1
#define TOTAL_GROUP_LOAD16_DATA_NUM   16
#define TOTAL_GROUP_LOAD8_DATA_NUM    8


typedef struct bcmi_ft_flowchecker_s {
    int bcmi_flowchecker_id;
    bcm_flowtracker_check_t flow_checker;
    struct bcmi_ft_flowchecker_s *next;
} bcmi_ft_flowchecker_t;


#define BCMI_FT_ALU_LOAD_MEMS         4


typedef struct alu_load_info_s {
    soc_mem_t mem;
    int mem_size;
    int pdd_bit;
} alu_load_info_t;

typedef struct bcmi_ft_group_pdd_bus_info_s {
    int param_id;
    int end;
    int start;
    int length;
} bcmi_ft_group_pdd_bus_info_t;

typedef struct alu_load_index_info_s {
     int total_mem_count;
     int total_idx_per_memory;
     SHR_BITDCL *index_bitmap;
     /* refcounts for all the indexes on this memory. */
     uint32 *idx_refcount;
     alu_load_info_t *alu_load_info;
     bcmi_ft_group_pdd_bus_info_t *pdd_bus_info;
     uint32 *group_fid_info;
} alu_load_index_info_t;



/* Chip Debug structure. */
typedef struct bcmi_ft_chip_debug_param_s {
    bcm_flowtracker_tracking_param_type_t param;
    bcmi_ft_alu_load_type_t alu_type;
    int mem_id;
    uint32 count;
/*    int count_installed;*/
} bcmi_ft_chip_debug_param_t;

typedef struct bcmi_ft_chip_debug_s {
    int chip_debug_enable;
    int num_debug_info;
    int alu32_mem_used[TOTAL_GROUP_ALU32_MEM];
    int alu16_mem_used[TOTAL_GROUP_ALU16_MEM];
    int alu32_ref_count[TOTAL_GROUP_ALU32_MEM];
    int alu16_ref_count[TOTAL_GROUP_ALU16_MEM];

/*
    int alu32_mem_used[TOTAL_GROUP_ALU32_MEM];
    int alu16_mem_used[TOTAL_GROUP_ALU16_MEM];
*/

    bcmi_ft_chip_debug_param_t *param_info;
} bcmi_ft_chip_debug_t;



extern bcmi_ft_chip_debug_t chip_debug_info[BCM_MAX_NUM_UNITS];

extern uint32 bcmi_dg_alu32_counters[TOTAL_GROUP_ALU32_MEM];
extern uint32 bcmi_dg_alu16_counters[TOTAL_GROUP_ALU16_MEM];

#define BCMI_FT_CHIP_DEBUG_INFO(unit) \
            (chip_debug_info[unit])

#define BCMI_FT_CHIP_DEBUG_ENABLE(unit) \
            ((chip_debug_info[unit]).chip_debug_enable)

#define BCMI_FT_CHIP_DEBUG_PARAM_INFO(unit) \
            (chip_debug_info[unit].param_info)

#define BCMI_FT_CHIP_DEBUG_PARAM(unit, id) \
            (chip_debug_info[unit].param_info[id])


/* There are 4 different types of memories. */
extern alu_load_index_info_t
    alu_load_index_info[BCM_MAX_NUM_UNITS][BCMI_FT_ALU_LOAD_MEMS];


#define BCMI_FT_ALU_INFO(unit, type) \
            (alu_load_index_info[unit][type])

#define BCMI_FT_ALU_LOAD_MEM_INFO(unit, type) \
            ((alu_load_index_info[unit][type]).alu_load_info)

#define BCMI_FT_GROUP_FID_INFO(unit, type) \
            ((alu_load_index_info[unit][type]).group_fid_info)

#define BCMI_FT_PDD_INFO(_u, type) \
            (alu_load_index_info[_u][type].pdd_bus_info)

#define BCMI_FT_ALU_LOAD_MEM_COUNT(unit, type) \
            ((alu_load_index_info[unit][type]).total_mem_count)

#define BCMI_FT_ALU_LOAD_REFCOUNT_INCR(_u, type, idx) \
            ((alu_load_index_info[_u][type].idx_refcount[idx])++)

#define BCMI_FT_ALU_LOAD_REFCOUNT_DEC(_u, type, idx) \
            ((alu_load_index_info[_u][type].idx_refcount[idx])--)

#define BCMI_FT_ALU_LOAD_REFCOUNT(_u, type, idx) \
            (alu_load_index_info[_u][type].idx_refcount[idx])

#define BCMI_FT_ALU_LOAD_REFCOUNT_INFO(_u, type, idx) \
            (alu_load_index_info[_u][type].idx_refcount)

#define BCMI_FT_ALU_LOAD_BITMAP(_u, type) \
            (alu_load_index_info[_u][type].index_bitmap)

#define BCMI_FT_ALU_LOAD_MEM_SIZE(_u, type, idx) \
            (((alu_load_index_info[_u][type]).alu_load_info[idx]).mem_size)


/* Flowtracker Software State. */
typedef struct bcmi_ft_flowchecker_sw_info_s {
    bcmi_flowtracker_flowchecker_info_t **bcmi_sw_info;
    SHR_BITDCL *ft_flowchecker_bitmap;
    uint32 *fc_idx_refcount;
    struct bcmi_ft_flowchecker_sw_info_s *next;
} bcmi_ft_flowchecker_sw_state_t;


extern bcmi_ft_flowchecker_sw_state_t
             bcmi_ft_flowchecker_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_FLOWCHECKER_BITMAP(unit)  \
            (bcmi_ft_flowchecker_state[unit].ft_flowchecker_bitmap)

#define BCMI_FT_FLOWCHECKER_STATE(unit)  \
            (bcmi_ft_flowchecker_state[unit].bcmi_sw_info)

#define BCMI_FT_FLOWCHECKER_INFO(unit, index)  \
            (bcmi_ft_flowchecker_state[unit].bcmi_sw_info[index])

#define BCMI_FT_FLOWCHECKER_REFCOUNT(unit, index)  \
            (bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])

#define BCMI_FT_FLOWCHECKER_REFCOUNT_DEC(unit, index)  \
            ((bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])--)

#define BCMI_FT_FLOWCHECKER_REFCOUNT_INC(unit, index)  \
            ((bcmi_ft_flowchecker_state[unit].fc_idx_refcount[index])++)

/* Internal Functions. */
int bcmi_ft_flowchecker_alu32_entry_install(
                int unit,
                bcmi_ft_group_alu_info_t info,
                soc_mem_t mem, int index);

int bcmi_ft_flowchecker_alu_base_control_set(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                uint32 *alu_fmt);

int bcmi_ft_flowchecker_alu16_entry_install(
                int unit,
                bcmi_ft_group_alu_info_t info,
                soc_mem_t mem, int index);

int
bcmi_ft_flowchecker_alu32_entry_uninstall(
                int unit,
                bcmi_ft_group_alu_info_t info,
                soc_mem_t mem, int index);


int
bcmi_ft_flowchecker_alu16_entry_uninstall(
                int unit,
                bcmi_ft_group_alu_info_t info,
                soc_mem_t mem, int index);

int
bcmi_ft_group_alu_load_configure(
               int unit, bcm_flowtracker_group_t id,
               bcmi_ft_group_alu_info_t *temp,
               bcmi_ft_alu_load_type_t alu_load_type,
               int index);

int
bcmi_ft_alu_entry_match(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              bcmi_ft_alu_hash_info_t *alu_info,
              int *load8_indexes, int *index);

int
bcmi_ft_load8_entry_match(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              bcmi_ft_alu_hash_info_t *alu_info,
              int *load8_indexes, int *index);

int
bcmi_ft_load16_entry_match(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              bcmi_ft_alu_hash_info_t *alu_info,
              int *load16_indexes, int *index);

/* Function pointer for the ALU/Load match routine. */
typedef int(*bcm_flowtracker_alu_load_entry_match)(
              int unit,
              bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              bcmi_ft_alu_hash_info_t *alu_info,
              int *load16_indexes, int *index);


/* Vector definitions for various ALU/LOAD methods. */

typedef struct bcmi_ft_alu_load_method_s {
    /* Lets start with ALU/LOAD matching. */
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


extern bcmi_ft_alu_load_method_t *alu_load_methods[BCMI_FT_ALU_LOAD_MEMS];

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


int
bcmi_ft_load16_entry_write(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              int *load16_indexes, int index);

int
bcmi_ft_load8_entry_write(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *temp,
              int *load8_indexes, int index);

int
bcmi_ft_load16_entry_install(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *info,
              int *indexes);

int
bcmi_ft_load8_entry_install(
              int unit, bcm_flowtracker_group_t id,
              bcmi_ft_group_alu_info_t *info,
              int *indexes);

int
bcmi_ft_alu_hw_operation_get(
                int unit,
                bcm_flowtracker_check_operation_t operation,
                int *check_0_operation,
                int *check_1_operation);

void
bcmi_ft_alu_load_mem_index_get(
                int unit,
                int index, bcmi_ft_alu_load_type_t type,
                int *new_index, soc_mem_t *mem, int *bit);

int
bcmi_ft_alu_check16_mapping_get(
                int unit,
                bcm_flowtracker_group_t id,
                bcm_flowtracker_check_t *check_map);

void
bcmi_ft_check_action_shift_bits_get(
                int unit,
                bcm_flowtracker_check_action_t action,
                bcmi_ft_group_alu_info_t *info,
                int *shift_bits);

extern int
bcmi_ft_alu_load_init(int unit);

extern int
bcmi_ft_alu_hash_init(int unit);

extern void
bcmi_ft_alu_hash_cleanup(int unit);

extern void
bcmi_ft_alu_load_cleanup(int unit);


/* Internal function headers. */


/* Functions for Hash Insert/Remove. */
int
bcmi_ft_alu_hash_insert(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                int *load_indexes,
                int *alu_load_index);


int
bcmi_ft_alu_hash_remove(
                int unit, bcm_flowtracker_group_t id,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                int *load_indexes,
                int *alu_load_index);

int
bcmi_ft_alu_hash_mem_index_get (
                int unit,  bcm_flowtracker_group_t id,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                int *load_indexes,
                int *alu_load_index);

/* Utility Functions. */
int
bcmi_ft_alu_check_free_index(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int index);

int
bcmi_ft_alu_get_free_index(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_alu_load_type_t alu_load_type,
            int *index);

int
bcmi_ft_alu_load_values_normalize(
                 int unit,
                 bcmi_ft_group_alu_info_t *info,
                 int *length,
                 int *alu);

int
bcmi_ft_alu_load_type_get(
                 int unit,
                 int alu,
                 int length,
                 bcmi_ft_alu_load_type_t *type);


extern int
bcmi_ft_alu_load_compare(
        int unit, bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type,
        bcmi_ft_alu_hash_info_t *alu_info,
        int *indexes);


int
bcmi_ft_alu_hw_operation_get(
                int unit,
                bcm_flowtracker_check_operation_t operation,
                int *check_0_operation,
                int *check_1_operation);

int
bcmi_ft_alu_hw_action_get(
                int unit,
                bcm_flowtracker_tracking_param_type_t param,
                bcm_flowtracker_check_action_t action,
                int * hw_action);

int
bcmi_ft_load16_entry_clear(int unit, int index);


int
bcmi_ft_load16_entry_uninstall(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);

int
bcmi_ft_load8_entry_uninstall(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);

int
bcmi_ft_alu32_entry_uninstall(
                int unit, bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);

int
bcmi_ft_alu16_entry_uninstall(
                int unit, bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);

int bcmi_ft_alu32_entry_install(
                int unit, bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);

int bcmi_ft_alu16_entry_install(
                int unit, bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *info,
                int *indexes);


extern int
bcmi_ft_check_compare(
        int unit,
        bcm_flowtracker_check_t check_id1,
        bcm_flowtracker_check_t check_id2);

extern int
bcmi_ft_group_alu_load_index_match(
            int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_alu_load_type_t alu_load_type,
            bcmi_ft_alu_hash_info_t *alu_info);

int
bcmi_ft_group_alu_load_trigger_set(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_flowtracker_direction_t direction);


/* Chip Debug methods. */


int bcmi_ft_chip_debug_info_set(
        int unit,
        int num_info,
        bcm_flowtracker_chip_debug_info_t *info);

int bcmi_ft_chip_debug_info_get(
        int unit,
        int max_size,
        bcm_flowtracker_chip_debug_info_t *info,
        int *num_debug_info);

int
bcmi_ft_tracking_param_in_Debug_mode(
                int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_group_alu_info_t *group_alu_info,
                bcmi_ft_alu_load_type_t alu_load_type);

int
bcmi_ft_chip_debug_alu_index_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type,
        int *index);

void
bcmi_ft_chip_debug_free_alu_index (
        int unit,
        bcmi_ft_group_alu_info_t *group_alu_info,
        bcmi_ft_alu_load_type_t alu_load_type);




int
bcmi_ft_chip_debug_counter_get(
        int unit,
        int mem_idx,
        bcmi_ft_alu_load_type_t alu_load_type,
        uint32 *count);


int
bcmi_ft_chip_debug_data_sanity(
        int unit,
        int num_info,
        bcm_flowtracker_chip_debug_info_t *info);

/* Get the matching data from valid pdd entry. */
int
bcmi_ft_group_alu_load_index_match_data_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int iter,
        bcmi_ft_group_alu_info_t **temp);
int
bcmi_ft_group_alu_load_pdd_status_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcmi_ft_alu_load_type_t alu_load_type,
        int iter);

int
bcmi_ft_flowchecker_list_add(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t flowchecker_id);


int
bcmi_ft_flowchecker_list_delete(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t flowchecker_id);

int
bcmi_ft_flowchecker_list_length_get(
        int unit,
        bcmi_ft_flowchecker_list_t **head);

int
bcmi_ft_flowchecker_opr_is_dual(
                int unit,
                bcm_flowtracker_check_operation_t operation);


int
bcmi_ft_check_list_compare(
        int unit,
        bcmi_ft_flowchecker_list_t **head,
        bcm_flowtracker_check_t check_id);


void
bcmi_ft_chip_debug_use_mem_update(
        int unit,
        bcmi_ft_alu_load_type_t alu_load_type,
        int mem_idx,
        int val);

int
bcmi_ft_chip_debug_mem_check(
        int unit,
        bcmi_ft_alu_load_type_t alu_load_type,
        int mem_idx);


int
bcmi_ft_group_clear(
        int unit,
        bcm_flowtracker_group_t id);

int
bcmi_ft_tracking_param_is_timestamp(
        int unit,
        bcm_flowtracker_tracking_param_type_t t_param);

extern int
bcmi_ft_alu_load_hash_dump(
    int unit);

#endif /* BCM_FLOWTRACKER_SUPPORT */

#endif /* _BCM_INT_FT_ALU_H_ */

