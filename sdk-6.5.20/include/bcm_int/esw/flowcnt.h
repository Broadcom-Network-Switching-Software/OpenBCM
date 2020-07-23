/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flowcnt.h
 * Purpose:     
 */
#ifndef _BCM_INT_FLOWCNT_H_
#define _BCM_INT_FLOWCNT_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <shared/idxres_afl.h>
#include <bcm/error.h>

 
/* ******************************************************************  */
/*              COMPOSITION OF STAT COUNTER ID                         */
/* ******************************************************************  */
/*              mode_id =  Max 3 bits (Total Eight modes)              */
/*              group_mode_id = Max 5 bits (Total 32)                  */
/*              pool_id = 4 (Max Pool:16)                              */
/*              a/c object_id=4 (Max Object:32)                        */
/*              15 bits for base index                                 */
/*              000    0-0000    0000   -00000     000-0000 0000-0000 */
/*              Mode3b Group5b   Pool4b -A/cObj4b  base-index          */
/* ****************************************************************  */
#define _BCM_FLOWCNT_MODE_START_BIT     29
#define _BCM_FLOWCNT_MODE_END_BIT       31
#define _BCM_FLOWCNT_GROUP_START_BIT    24
#define _BCM_FLOWCNT_GROUP_END_BIT      28
#define _BCM_FLOWCNT_POOL_START_BIT     20
#define _BCM_FLOWCNT_POOL_END_BIT       23
#define _BCM_FLOWCNT_OBJECT_START_BIT   15
#define _BCM_FLOWCNT_OBJECT_END_BIT     19
#define _BCM_FLOWCNT_BASE_IDX_START_BIT  0
#define _BCM_FLOWCNT_BASE_IDX_END_BIT   13

#define _BCM_FLOWCNT_MODE_MASK \
    ((1<<(_BCM_FLOWCNT_MODE_END_BIT - _BCM_FLOWCNT_MODE_START_BIT+1))-1)
#define _BCM_FLOWCNT_GROUP_MASK \
    ((1<<(_BCM_FLOWCNT_GROUP_END_BIT - _BCM_FLOWCNT_GROUP_START_BIT+1))-1)
#define _BCM_FLOWCNT_POOL_MASK \
    ((1<<(_BCM_FLOWCNT_POOL_END_BIT - _BCM_FLOWCNT_POOL_START_BIT+1))-1)
#define _BCM_FLOWCNT_OBJECT_MASK \
    ((1<<(_BCM_FLOWCNT_OBJECT_END_BIT - _BCM_FLOWCNT_OBJECT_START_BIT+1))-1)
#define _BCM_FLOWCNT_BASE_IDX_MASK \
    ((1<<(_BCM_FLOWCNT_BASE_IDX_END_BIT-_BCM_FLOWCNT_BASE_IDX_START_BIT+1))-1)

#define _BCM_STAT_FLOWCNT_MAX_PROFILES      (2)
#define _BCM_STAT_FLOWCNT_MAX_PROFILE_GROUP (4)
#define _BCM_STAT_FLOWCNT_MAX_SELECTORS     (16)

typedef enum _bcm_flowcnt_stat_type_e {
    bcmFlowcntStatTypePacket = 1,
    bcmFlowcntStatTypeByte = 2    
}_bcm_flowcnt_stat_type_t;

typedef struct _bcm_flowcnt_group_mode_attr_selector_s {
    uint32 counter_offset;              /* Counter Offset */
    bcm_stat_group_mode_attr_t attr;    /* Attribute Selector */
    uint32 attr_value;                  /* Attribute Values */
}_bcm_flowcnt_group_mode_attr_selector_t;

typedef struct  _bcm_esw_flowcnt_profile_group_s {
    uint32        available;
    uint32        reference_count;
    uint32        total_counters;
    bcm_stat_group_mode_t    group_mode;    
    uint32        flags;    
    uint32        num_selectors;
    _bcm_flowcnt_group_mode_attr_selector_t *attr_selectors;
}_bcm_esw_flowcnt_profile_group_t;

typedef struct  _bcm_esw_flowcnt_profile_s {
    soc_profile_mem_t   profile_mem;
    soc_mem_t   profile_table;
    bcm_stat_group_mode_attr_t  attr;
    bcm_stat_flex_direction_t   direction;
    int num_entries; /* numbers of entries per group */
    int num_groups;  /* numbers of group per profile */
    _bcm_esw_flowcnt_profile_group_t *flowcnt_group;
}_bcm_esw_flowcnt_profile_t;

typedef struct  _bcm_esw_flowcnt_pool_stat_s {
    uint32  used_entries;
    uint32  attached_entries;
}_bcm_esw_flowcnt_pool_stat_t;

typedef struct _bcm_esw_flowcnt_pool_s {
    soc_mem_t   mem;
    shr_aidxres_list_handle_t flowcnt_aidxres_list_handle;    
    uint32 *flow_packet_counter;
    uint64 *flow_byte_counter;
    uint64 *flow_packet64_counter;    
    uint16 *flow_base_index_reference_count;
    uint16 *associate_profile_group;
    _bcm_flowcnt_stat_type_t  *flow_stat_type;
    bcm_stat_object_t *object;
    _bcm_esw_flowcnt_pool_stat_t  pool_stat;
}_bcm_esw_flowcnt_pool_t;

typedef struct  _bcm_flowcnt_control_s {
    int init;
    sal_mutex_t flowcnt_mutex;
    uint32 *default_attr_mode_id[_BCM_STAT_FLOWCNT_MAX_PROFILES];
    /* chip specific */
    _bcm_esw_flowcnt_profile_t *flowcnt_profile;
    int num_profiles;
    _bcm_esw_flowcnt_pool_t *flowcnt_pool;
    int num_pools;
    int counter_block; /* max counter offset value in power of two */
}_bcm_esw_flowcnt_control_t;


typedef enum _bcm_flowcnt_group_mode_e {
   bcmFlowcntProfileGroup1 = bcmStatGroupModeCount,
   bcmFlowcntProfileGroup2, 
   bcmFlowcntProfileGroup3, 
   bcmFlowcntProfileGroup4 
}_bcm_flowcnt_group_mode_t;


#ifdef BCM_WARM_BOOT_SUPPORT
typedef struct _bcm_flowcnt_warmboot_info_s {
    uint32 default_attr_mode_id
           [_BCM_STAT_FLOWCNT_MAX_PROFILES][bcmStatGroupModeAttrMaxValue];
}_bcm_flowcnt_warmboot_info_t;

typedef struct _bcm_flowcnt_warmboot_group_info_s {
    uint32 group_mode;
    uint32 flags;
    uint32 total_counters;
    uint32 num_selectors;
    _bcm_flowcnt_group_mode_attr_selector_t attr_selectors
                                            [_BCM_STAT_FLOWCNT_MAX_SELECTORS];
}_bcm_flowcnt_warmboot_group_info_t;
#endif
    
extern int _bcm_esw_flowcnt_init(int unit);
extern int _bcm_esw_flowcnt_cleanup(int unit);
extern int _bcm_esw_flowcnt_group_create(int unit, bcm_stat_object_t object,
                bcm_stat_group_mode_t group_mode, uint32 *stat_counter_id,
                uint32 *num_entries);
extern int _bcm_esw_flowcnt_object_table_attach(int unit, 
                bcm_stat_object_t object, soc_mem_t table, uint32 index,
                int profile_group, uint32 base_idx, uint32 pool_idx,
                void *entry_data1);
extern void _bcm_esw_flowcnt_counter_id_get(uint32 stat_counter_id,
                bcm_stat_group_mode_t *group, bcm_stat_object_t *object,
                uint32 *profile_group, uint32 *pool_idx,
                uint32 *base_idx);
extern int _bcm_esw_flowcnt_counter_set(int unit, uint32 index,
                soc_mem_t table, uint32 byte_flag, uint32 counter_index,
                bcm_stat_value_t *value);
extern int _bcm_esw_flowcnt_counter_get(int unit, int sync_mode,
                uint32 index, soc_mem_t table, uint32 byte_flag,
                uint32 counter_index, bcm_stat_value_t *value);              
extern int _bcm_esw_flowcnt_group_mode_id_create(int unit, uint32 flags,
                uint32 total_counters, uint32 num_selectors,
                bcm_stat_group_mode_attr_selector_t *attr_selectors,
                uint32 *mode_id);
extern int _bcm_esw_flowcnt_group_mode_id_get(int unit, uint32 mode_id,
                uint32 *flags, uint32 *total_counters,
                uint32 num_selectors, 
                bcm_stat_group_mode_attr_selector_t *attr_selectors,
                uint32 *actual_num_selectors);
extern int _bcm_esw_flowcnt_group_mode_id_destroy(int unit, uint32 mode_id);
extern int _bcm_esw_flowcnt_custom_group_create(int unit, uint32 mode_id,
                bcm_stat_object_t object, uint32 *stat_counter_id,
                uint32 *num_entries);
extern int _bcm_esw_flowcnt_object_table_detach(int unit, soc_mem_t ingress_table,
                uint32 index);
extern int _bcm_esw_flowcnt_stat_id_get(int unit, soc_mem_t mem, int index,
                uint32 *stat_counter_id);
extern int _bcm_esw_flowcnt_stat_type_validate(int unit, soc_mem_t table,
                int index, _bcm_flowcnt_stat_type_t flow_stat_type);
extern int _bcm_esw_flowcnt_group_destroy(int unit, uint32 stat_counter_id);
extern int _bcm_esw_flowcnt_group_dump_info(int unit, int all_flag,
                bcm_stat_object_t object, bcm_stat_group_mode_t group);
extern int _bcm_esw_flowcnt_counter_raw_set(int unit,
                uint32 stat_counter_id, uint32 byte_flag,
                uint32 counter_index, bcm_stat_value_t *value);
extern int _bcm_esw_flowcnt_counter_raw_get(int unit, int sync_mode,
                uint32 stat_counter_id, uint32 byte_flag,
                uint32 counter_index, bcm_stat_value_t *value);
extern int _bcm_esw_flowcnt_total_counters_get(int unit,
                uint32 stat_counter_id, uint32 *total_counters);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_flowcnt_sync(int unit);
#endif
#endif  /* !_BCM_INT_FLOWCNT_H_ */
