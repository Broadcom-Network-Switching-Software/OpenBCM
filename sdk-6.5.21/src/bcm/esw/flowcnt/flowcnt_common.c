/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flowcnt_common.c
 * Purpose:     Manage common functionality for flow counter implementation
 */

#include <shared/bsl.h>

#include <bcm/stat.h>
#include <bcm/module.h>
#include <bcm_int/esw/flowcnt.h>
#include <bcm_int/esw/stat.h>
#include <soc/scache.h>


static _bcm_esw_flowcnt_control_t *flowcnt_control[SOC_MAX_NUM_DEVICES]={NULL};

typedef struct {
    uint32 entry_data[BYTES2WORDS(5)];
}counter_table_entry_t;

#define MAX_FLOWCNT_COUNTER_TABLE_SIZE  (512)

static counter_table_entry_t *flowcnt_temp_counter[SOC_MAX_NUM_DEVICES] = {NULL};

/* for those counter indexes created but not attached */
#define MAX_FLOWCNT_SCACHE_SIZE (32)
static uint8 *local_scache_ptr[SOC_MAX_NUM_DEVICES]={NULL};
static uint32 local_scache_size=0;

#define FLOWCNT_SCACHE_SIZE_1 (MAX_FLOWCNT_SCACHE_SIZE * sizeof(uint32))
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_3 SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_10 SOC_SCACHE_VERSION(1,10)
static soc_scache_handle_t handle=0;
static uint32 flowcnt_scache_allocated_size[SOC_MAX_NUM_DEVICES]= {0};
static uint8 *flowcnt_scache_ptr[SOC_MAX_NUM_DEVICES]={NULL};
STATIC int _bcm_esw_flowcnt_warmboot_info_recover(int unit,uint8 *scache_ptr);
STATIC int _bcm_esw_flowcnt_warmboot_object_retrive(int unit);
/* scache layout of flowcnt
 *   1. stat_counter_id
 *   2. default_attr_mode_id (size maybe change because of
 *                            bcmStatGroupModeAttrMaxValue)
 *          local   : the size according the current mode attr max value
 *          recover : the size in scache
 *             when firmware upgrade, "local" maybe be larger than "recover"
 *             when firmware downgrade, "local" maybe be less than "recover"
 *   3. flowcnt_group_info
 */
#define FLOWCNT_SCACHE_SIZE_2_LOCAL (sizeof(_bcm_flowcnt_warmboot_info_t))
#define FLOWCNT_SCACHE_SIZE_2_RECOVER(unit) (flowcnt_scache_allocated_size[unit]\
                                       - FLOWCNT_SCACHE_SIZE_1\
                                       - FLOWCNT_SCACHE_SIZE_3)
#define FLOWCNT_SCACHE_SIZE_3 (sizeof(_bcm_flowcnt_warmboot_group_info_t) *\
                              _BCM_STAT_FLOWCNT_MAX_PROFILE_GROUP *\
                              _BCM_STAT_FLOWCNT_MAX_PROFILES)
#endif

#define _BCM_FLOWCNT_MODE_ID_VALID  (0x80000000)

/* need to add bias to tell if value of the pgroup is valid*/
#define _BCM_FLOWCNT_PGROUP_BIAS   (0xF)
/*
 * Macro:
 *     _FLOWCNT_IS_INIT (internal)
 * Purpose:
 *     Check that the unit is valid and confirm that the oam functions
 *     are initialized.
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNIT), return(BCM_E_UNAVAIL), or
 *     return(BCM_E_INIT) if fails.
 */
#define _FLOWCNT_IS_INIT(unit)                                               \
            do {                                                             \
                if (!soc_feature(unit, soc_feature_flowcnt)) {               \
                    return (BCM_E_UNAVAIL);                                  \
                }                                                            \
                if (flowcnt_control[unit] == NULL) {                            \
                    LOG_ERROR(BSL_LS_BCM_FLEXCTR, \
                              (BSL_META_U(unit, \
                                          "Flowcnt Error: Module not initialized\n"))); \
                    return (BCM_E_INIT);                                     \
                }                                                            \
            } while (0)

/*
 * Macro:
 *     _FLOWCNT_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _unit_    - Unit.
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 *    _dma_     - use sal_alloc or soc_cm_alloc.
 *    _rv_      - return value
 */
#define _FLOWCNT_ALLOC(_unit_,_ptr_,_ptype_,_size_,_descr_,_dma_,_rv_) \
            do { \
                if (NULL == (_ptr_)) { \
                    if (0 == (_dma_)) { \
                        (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                    } else { \
                        (_ptr_) = (_ptype_ *) soc_cm_salloc((_unit_),(_size_), \
                                                            (_descr_)); \
                    } \
                } \
                if((_ptr_) != NULL) { \
                    sal_memset((_ptr_), 0, (_size_)); \
                    _rv_ = BCM_E_NONE; \
                }  else { \
                    LOG_ERROR(BSL_LS_BCM_FLEXCTR, \
                              (BSL_META("Flowcnt Error: Allocation failure %s\n"), \
                               (_descr_))); \
                    _rv_ = BCM_E_MEMORY; \
                } \
            } while (0)

#define _FLOWCNT_FREE(_unit, _ptr, _dma) \
            do {\
                    if (0 == (_dma)) { \
                        sal_free((_ptr)); \
                    } else { \
                        soc_cm_sfree((_unit), (_ptr)); \
                    }\
                } while (0)

#define _FLOWCNT_LOCK(fc) \
        sal_mutex_take((fc)->flowcnt_mutex, sal_mutex_FOREVER);
#define _FLOWCNT_UNLOCK(fc) \
        sal_mutex_give((fc)->flowcnt_mutex);

#define _FLOWCNT_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef struct flowcnt_profile_table_info_s {
    soc_mem_t   mem;        /* profile mem */
    int         num_groups;  /* numbers of group per profile */
    bcm_stat_group_mode_attr_t  attr;
    bcm_stat_flex_direction_t   direction;
} flowcnt_profile_table_info_t;

typedef struct flowcnt_object_table_fields_s {
    char    *key_type;
    soc_field_t base_index_field;
    soc_field_t profile_ptr_field;
} flowcnt_object_table_fields_t;

typedef struct flowcnt_object_info_s {
    bcm_stat_object_t   object;
    soc_mem_t   object_mem;
    soc_mem_t   offset_mem;
    soc_mem_t   counter_mem;
    soc_field_t view_field;
    flowcnt_object_table_fields_t counter_fields;
}flowcnt_object_info_t;

/* 
  * Normally reserve_count is 1. i.e. reserve index 0 for warmboot 
  */
typedef struct flowcnt_pool_table_info_s {
    soc_mem_t pool_mem;
    uint32    reserve_base;
    uint32    reserve_count;
}flowcnt_pool_table_info_t;

static flowcnt_profile_table_info_t _hr3_flowcnt_profile_table[] = {
    {ING_VLAN_COUNTER_PRI_COS_MAPm, 4, 
        bcmStatGroupModeAttrPktCfiPri, bcmStatFlexDirectionIngress},
    {EGR_VLAN_COUNTER_PRI_COS_MAPm, 4, 
        bcmStatGroupModeAttrPktCfiPri, bcmStatFlexDirectionEgress},
};

/* in HR3, reserve indx0-7 for ambiguous count */
static flowcnt_pool_table_info_t _hr3_flowcnt_pool_table[] = {
    {ING_VLAN_COUNTER_TABLEm, 0, 8},
    {EGR_VLAN_COUNTER_TABLEm, 0, 8},
};

static int _hr3_stat_group_mode_info[] = {
    bcmStatGroupModeSingle,
    -1
};

static flowcnt_object_info_t _hr3_stat_object_info[] = {
    {bcmStatObjectIngVlanXlate, VLAN_XLATEm, 
        ING_VLAN_COUNTER_PRI_COS_MAPm,
        ING_VLAN_COUNTER_TABLEm, KEY_TYPEf,
        {"XLATE", XLATE__VLAN_CTR_BASE_INDEXf, XLATE__VLAN_CTR_PROFILE_PTRf}
        },
    {bcmStatObjectIngFieldStageLookup, VFP_POLICY_TABLEm,
        ING_VLAN_COUNTER_PRI_COS_MAPm,
        ING_VLAN_COUNTER_TABLEm, -1,
        {"", VLAN_CTR_BASE_INDEXf, VLAN_CTR_PROFILE_PTRf}
        },
    {bcmStatObjectEgrVlanXlate, EGR_VLAN_XLATEm, 
        EGR_VLAN_COUNTER_PRI_COS_MAPm,
        EGR_VLAN_COUNTER_TABLEm, ENTRY_TYPEf,
        {"XLATE", XLATE__VLAN_CTR_BASE_INDEXf, XLATE__VLAN_CTR_PROFILE_PTRf}
        },
    {-1, -1, -1, -1, -1,{NULL,-1,-1}}
};

static flowcnt_pool_table_info_t *flowcnt_pool_table[SOC_MAX_NUM_DEVICES] = {NULL};
static flowcnt_profile_table_info_t *flowcnt_profile_table[SOC_MAX_NUM_DEVICES] = {NULL};
static int *valid_stat_group_mode_list[SOC_MAX_NUM_DEVICES] = {NULL};
static flowcnt_object_info_t *valid_stat_object_list[SOC_MAX_NUM_DEVICES] = {NULL};


STATIC int
_bcm_esw_flowcnt_control_get(int unit, _bcm_esw_flowcnt_control_t **fc)
{
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }
    /* Ensure flowcnt module is initialized. */
    _FLOWCNT_IS_INIT(unit);

    *fc = flowcnt_control[unit];

    return (BCM_E_NONE);
}

STATIC 
int _bcm_esw_flowcnt_stat_object_validate(
    int unit, 
    bcm_stat_object_t object)
{
    int i;
    int rv = BCM_E_PARAM;
    flowcnt_object_info_t *stat_object_list;

    if (valid_stat_object_list[unit] == NULL){
        return BCM_E_INTERNAL;
    }
    stat_object_list = valid_stat_object_list[unit];
    for (i = 0; stat_object_list[i].object != -1; i++) {
        if (stat_object_list[i].object == object){
            rv = BCM_E_NONE;
            break;
        }
    }

    return rv;

}

STATIC int 
_bcm_esw_flowcnt_stat_group_mode_validate(int unit,
                bcm_stat_group_mode_t group_mode)
{
    int i;
    int rv = BCM_E_PARAM;

    if (valid_stat_group_mode_list[unit] == NULL){
        return BCM_E_INTERNAL;
    }
    for (i = 0; valid_stat_group_mode_list[unit][i] != -1; i++) {
        if (valid_stat_group_mode_list[unit][i] == group_mode){
            rv = BCM_E_NONE;
            break;
        }
    }
    return rv;
}

STATIC int
_bcm_hr3_flowcnt_ctrl_init(int unit, _bcm_esw_flowcnt_control_t *fc)
{
    int rv;
    valid_stat_group_mode_list[unit] = _hr3_stat_group_mode_info;
    valid_stat_object_list[unit] = _hr3_stat_object_info;
    flowcnt_profile_table[unit] = _hr3_flowcnt_profile_table;
    flowcnt_pool_table[unit] = _hr3_flowcnt_pool_table;
    fc->num_profiles = _FLOWCNT_ARRAY_SIZE(_hr3_flowcnt_profile_table);
    fc->num_pools = _FLOWCNT_ARRAY_SIZE(_hr3_flowcnt_pool_table);
    /* The max value of the offset field on ING/EGR_VLAN_COUNTER_PRI_COS_MAP 
       in power of two */
    fc->counter_block = 3;
    _FLOWCNT_ALLOC(unit, flowcnt_temp_counter[unit], counter_table_entry_t,
                   MAX_FLOWCNT_COUNTER_TABLE_SIZE *
                   sizeof(counter_table_entry_t),
                   "temp_counter", 1, rv);

    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_object_table_index_get
 * Description:
 *      Get the object table index based on the object mem informantion based
 *      on the object or object_mem.
 * Parameters:
 *      unit                - (IN) unit number
 *      index               - (IN) index in the object mem.
 *      table               - (IN) object memory.
 *      data                - (IN) entry data
 *      object_index        - (OUT) valid_stat_object_list index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_flowcnt_object_table_index_get(
    int       unit,
    uint32    index,
    soc_mem_t table,
    void      *data,
    int *object_index)
{
    int i;
    soc_field_t view_field = INVALIDf;
    soc_mem_info_t *memp;
    uint32 key_type=0;
    flowcnt_object_info_t *stat_object_list;
    flowcnt_object_table_fields_t   *o_fields;

    *object_index = -1;
    stat_object_list = valid_stat_object_list[unit];
    for (i = 0; stat_object_list[i].object_mem != -1; i++) {
        if (stat_object_list[i].object_mem == table) {
            memp = &SOC_MEM_INFO(unit, table);
            view_field = stat_object_list[i].view_field;
            o_fields = &stat_object_list[i].counter_fields;
            if (view_field == INVALIDf) {
                *object_index = i;
                break;
            } else {
                key_type = soc_mem_field32_get(unit, table, data, view_field);
                LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                          (BSL_META_U(unit,
                                      "key_type %d "),
                          key_type));
                if (sal_strcmp(memp->views[key_type], o_fields->key_type) == 0) {
                    *object_index = i;
                    break;
                }
            }
        }
    }
    if (*object_index == -1) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_esw_flowcnt_object_table_fields_get(
    int       unit,
    uint32    index,
    soc_mem_t table,
    void      *data,
    soc_field_t    *profile_ptr_field,
    soc_field_t    *base_idx_field)
{
    int object_index=-1,rv=BCM_E_NONE;
    flowcnt_object_table_fields_t   *o_fields;
    flowcnt_object_info_t *stat_object_list;

    rv = _bcm_esw_flowcnt_object_table_index_get(
            unit, index, table, data, &object_index);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    stat_object_list = valid_stat_object_list[unit];
    o_fields = &stat_object_list[object_index].counter_fields;

    *profile_ptr_field = o_fields->profile_ptr_field;
    *base_idx_field = o_fields->base_index_field;

    /* Just a safety check */
    if ((soc_mem_field_valid(unit,table,*profile_ptr_field) == FALSE) ||
        (soc_mem_field_valid(unit,table,*base_idx_field) == FALSE)){
        LOG_WARN(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                             "INTERNAL Error i.e. "
                              "required offset,base_idx fields are "
                              "not valid \n")));
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_esw_flowcnt_object_table_fields_values_get(
    int       unit,
    uint32    index,
    soc_mem_t table,
    void      *data,
    uint32    *profile_ptr,
    uint32    *base_idx)
{
    int rv = BCM_E_NONE;
    soc_field_t profile_ptr_field = INVALIDf;
    soc_field_t base_index_field = INVALIDf;

    rv = _bcm_esw_flowcnt_object_table_fields_get(
         unit, index, table, data, &profile_ptr_field,
         &base_index_field);

    if (BCM_FAILURE(rv)){
        return rv;
    }
    *profile_ptr =  soc_mem_field32_get(unit,table,data,profile_ptr_field);
    *base_idx =  soc_mem_field32_get(unit,table,data,base_index_field);
    return rv;
}

STATIC int
_bcm_esw_flowcnt_object_table_fields_values_set(
    int       unit,
    uint32    index,
    soc_mem_t table,
    void      *data,
    uint32    profile_ptr,
    uint32    base_idx)
{
    int rv = BCM_E_NONE;
    soc_field_t profile_ptr_field = INVALIDf;
    soc_field_t base_index_field = INVALIDf;

    rv = _bcm_esw_flowcnt_object_table_fields_get(
         unit, index, table, data, &profile_ptr_field,
         &base_index_field);

    if (BCM_FAILURE(rv)){
        return rv;
    }
    soc_mem_field32_set(unit,table,data,profile_ptr_field,profile_ptr);
    soc_mem_field32_set(unit,table,data,base_index_field,base_idx);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_stat_id_insert
 * Description:
 *      Inserts stat id in local scache table. Useful for WARM-BOOT purpose
 * Parameters:
 *      scache_ptr            - (IN) Local scache table pointer
 *      stat_counter_id       - (IN) Flex Stat Counter Id
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static int
_bcm_esw_flowcnt_stat_id_insert(uint8 *scache_ptr, uint32 stat_counter_id)
{
    uint32 index = 0;
    uint32 *ptr = (uint32 *)scache_ptr;
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META("Inserting %d "),
               stat_counter_id));
    for (index = 0; index < MAX_FLOWCNT_SCACHE_SIZE; index++) {
         if (ptr[index] == 0) {
             LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                       (BSL_META("Inserted \n")));
             ptr[index] = stat_counter_id;
             break;
         }
    }
    if (index == MAX_FLOWCNT_SCACHE_SIZE) {
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_flowcnt_stat_id_delete
 * Description:
 *      Deletes stat id from local scache table. Useful for WARM-BOOT purpose
 * Parameters:
 *      scache_ptr            - (IN) Local scache table pointer
 *      stat_counter_id       - (IN) Flex Stat Counter Id
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
static int
_bcm_esw_flowcnt_stat_id_delete(uint8 *scache_ptr,uint32 stat_counter_id)
{
    uint32 index = 0;
    uint32 *ptr = (uint32 *)scache_ptr;
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META("Deleting ID:%d "),
               stat_counter_id));
    for (index = 0; index < MAX_FLOWCNT_SCACHE_SIZE; index++) {
         if (ptr[index] == stat_counter_id) {
             LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                       (BSL_META("Deleted \n")));
             ptr[index] = 0;
             break;
         }
    }
    if (index == MAX_FLOWCNT_SCACHE_SIZE) {
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_flowcnt_accounting_object_info_get
 * Description:
 *      Get object table info for given accounting table and index
 * Parameters
 *      unit                  - (IN) unit number
 *      table                 - (IN) Accounting Tables
 *      index                 - (IN) index of the accounting tables 
 *      profile_ptr           - (OUT) profile group mode
 *      base_index            - (OUT) base index
 *      object_index          - (OUT) index to internal object table 
 *                                      valid_stat_object_list
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int 
_bcm_esw_flowcnt_accounting_object_info_get(
            int         unit,
            soc_mem_t   table,
            int         index,
            uint32      *profile_ptr,
            uint32      *base_index,
            int         *object_index)
{
    soc_mem_info_t *memp = NULL;
    int rv = BCM_E_NONE;
    void *entry_data = NULL;
    uint32 entry_data_size = 0;

    memp = &SOC_MEM_INFO(unit, table);
    entry_data_size = WORDS2BYTES(BYTES2WORDS(memp->bytes));
    _FLOWCNT_ALLOC(unit, entry_data, void, entry_data_size,
               "object table entry", 0, rv);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    if (soc_mem_read(unit, table, MEM_BLOCK_ANY,
                     index,
                     entry_data) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Read failure for Table %s with index %d \n"),
                   SOC_MEM_UFNAME(unit, table),index));
        _FLOWCNT_FREE(unit, entry_data, 0);
        return  BCM_E_INTERNAL;
    }
    if (soc_mem_field_valid(unit,table,VALIDf)) {
        if (soc_mem_field32_get(unit, table, entry_data, VALIDf)==0) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Table %s  with index %d is Not valid \n"),
                       SOC_MEM_UFNAME(unit, table),index));
            _FLOWCNT_FREE(unit, entry_data, 0);
            return BCM_E_PARAM;
        }
    }

    rv = _bcm_esw_flowcnt_object_table_fields_values_get(
            unit, index, table, entry_data,
            profile_ptr, base_index);
    if (BCM_FAILURE(rv)){
        _FLOWCNT_FREE(unit, entry_data, 0);
        return rv;
    }
    rv = _bcm_esw_flowcnt_object_table_index_get(
            unit, index, table, entry_data, object_index);
    if (BCM_FAILURE(rv)){
        _FLOWCNT_FREE(unit, entry_data, 0);
        return rv;
    }
    _FLOWCNT_FREE(unit, entry_data, 0);

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_flowcnt_ctrl_common_init
 * Description:
 *      Initialize and allocate all required resources for Hurricane3
 * Parameters:
 *      unit                  - (IN) unit number
 * Return Value:
 *      BCM_E_XXX
 *                            - if resource allocation errored, just returned.
 *                              Resource free should be taken care in upper layer.
 * Notes:
 *   
 *   _bcm_esw_flowcnt_profile_t *flowcnt_profile;
 *   int num_profiles;
 *   _bcm_esw_flowcnt_pool_t *flowcnt_pool;
 *   int num_pool;
 *   Above should be allocated and assigned per chip profile table.
 *
 *   shr_aidxres_list_handle_t flowcnt_aidxres_list_handle;
 *   uint32 flow_packet_counter;
 *   uint64 flow_byte_counter;
 *   uint64 flow_packet64_counter;
 *   uint16 flow_base_index_reference_count; 
 *   Above should be allocated and assigned per chip counter table.
 */
STATIC int
_bcm_flowcnt_ctrl_common_init(int unit, _bcm_esw_flowcnt_control_t *fc)
{
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    int i;
    int rv = BCM_E_NONE;
    soc_mem_t   mem;
    soc_mem_info_t *memp;
    int entry_words, num_ctr;
    flowcnt_profile_table_info_t  *profile_table;
    flowcnt_pool_table_info_t *pool_table;

    if (fc == NULL) {
        return BCM_E_MEMORY;
    }
    /* Allocate default attr mode id */
    for (i = 0; i < fc->num_profiles; i++) {
        _FLOWCNT_ALLOC(unit, fc->default_attr_mode_id[i], uint32,
                       bcmStatGroupModeAttrMaxValue * sizeof(uint32),
                       "Flowcnt default attr mode_id", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Allocate the profile resource */
    _FLOWCNT_ALLOC(unit, fc->flowcnt_profile, _bcm_esw_flowcnt_profile_t,
                   fc->num_profiles * sizeof(_bcm_esw_flowcnt_profile_t),
                   "Flowcnt profile", 0, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    profile_table = flowcnt_profile_table[unit];
    for (i = 0; i < fc->num_profiles; i++) {
        fc_profile = &fc->flowcnt_profile[i];
        mem = profile_table[i].mem;
        memp = &SOC_MEM_INFO(unit, mem);
        entry_words = BYTES2WORDS(memp->bytes);
        soc_profile_mem_t_init(&fc_profile->profile_mem);
        rv = soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                    &fc_profile->profile_mem);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Flowcnt profile create - Failed.\n")));
            return (rv);
        }
        fc_profile->num_groups = profile_table[i].num_groups;
        _FLOWCNT_ALLOC(unit, fc_profile->flowcnt_group,
                   _bcm_esw_flowcnt_profile_group_t,
                   fc_profile->num_groups *
                   sizeof(_bcm_esw_flowcnt_profile_group_t),
                   "Flowcnt profile group", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        fc_profile->direction = profile_table[i].direction;
        fc_profile->attr = profile_table[i].attr;
        fc_profile->profile_table = mem;
        fc_profile->num_entries = soc_mem_index_count(unit, mem) /
                                    profile_table[i].num_groups;
    }

    /* Allocate the counter table resource */
    _FLOWCNT_ALLOC(unit, fc->flowcnt_pool, _bcm_esw_flowcnt_pool_t,
                   fc->num_pools * sizeof (_bcm_esw_flowcnt_pool_t),
                   "Flowcnt pool", 0, rv);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    pool_table = flowcnt_pool_table[unit];
    for (i = 0; i < fc->num_pools; i++) {
        fc_pool = &fc->flowcnt_pool[i];
        fc_pool->mem = pool_table[i].pool_mem;
        num_ctr = soc_mem_index_count(unit, fc_pool->mem);

        _FLOWCNT_ALLOC(unit, fc_pool->flow_byte_counter,
                       uint64,
                       sizeof(uint64) * num_ctr,
                       "Flow byte counter", 1, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->flow_packet64_counter,
                       uint64,
                       sizeof(uint64) * num_ctr,
                       "Flow packet64 counter", 1, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->flow_packet_counter,
                       uint32,
                       sizeof(uint32) * num_ctr,
                       "Flow packet counter", 1, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->flow_base_index_reference_count,
                       uint16,
                       sizeof(uint16) * num_ctr,
                       "Flow BaseIndexAllocation", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->associate_profile_group,
                       uint16,
                       sizeof(uint16) * num_ctr,
                       "Flow associate_profile_group", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->object,
                       bcm_stat_object_t,
                       sizeof(bcm_stat_object_t) * num_ctr,
                       "Flow stat object", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        _FLOWCNT_ALLOC(unit, fc_pool->flow_stat_type,
                       _bcm_flowcnt_stat_type_t, 
                       sizeof(_bcm_flowcnt_stat_type_t) * num_ctr,
                       "Flow stat type", 0, rv);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        if (shr_aidxres_list_create(
             &fc_pool->flowcnt_aidxres_list_handle,
             0, num_ctr-1,
             0, num_ctr-1,
             fc->counter_block, 
             "flow-counter") != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                    (BSL_META_U(unit,
                                "Unrecoverable error. "
                                "Couldn't Create AllignedList:"
                                "FlowCounter\n")));
            return BCM_E_INTERNAL;
        }
        /* reserve first counter entry for warmboot recovery */
        shr_aidxres_list_reserve_block(
            fc_pool->flowcnt_aidxres_list_handle,
            pool_table->reserve_base,
            pool_table->reserve_count);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_collect
 * Description:
 *      Flex Counter accumulation routine called periodically
 *      to sync s/w copy with h/w copy.
 * Parameters:
 *      unit                  - (IN) unit number
 *      mem                   - (IN) Ingress or Egress flow counter memory
 *                                   if -1 Both Ingress and Egress memories
 *                                   else Ingress or Egress pool.
 *      counter_idx           - (IN) counter index.
 *                                   if -1 accumulate for all counters
 *                                   else retrieve a specific counter.
 * Return Value:
 *      None
 * Notes:
 *       This routine accumulates all the flow counters and
 *       update the software copy of the flow counters.
 *       Counter thread invokes periodically this routine.
 *       If counter index is not -1 then only a specific counter
 *       value is read from harware and synced to the software copy.
 *       used by all the counter_sync_get API's
 */
STATIC void
_bcm_esw_flowcnt_counter_collect(int unit,
                                 soc_mem_t mem,
                                 uint32 counter_idx)
{

    uint32 packet_count=0, count_mode;
    uint64 byte_count;
    uint32 mem_index_min;
    uint32 mem_index_max;
    uint32 zero=0;
    uint32 one=1;
    uint64 packet64_count;
    uint64 prev_masked_byte_count, prev_masked_packet64_count;
    uint64 max_byte_size, max_byte_mask, max_packet64_size, max_packet64_mask;
    soc_memacc_t memacc_pkt;
    soc_memacc_t memacc_byte;
    soc_memacc_t memacc_countmode;
    soc_mem_t l_mem;
    int i, index;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    (void)_bcm_esw_flowcnt_control_get(unit, &fc);

    if (fc == NULL) {
        return;
    }

    COMPILER_64_ZERO(byte_count);
    COMPILER_64_ZERO(prev_masked_byte_count);
    COMPILER_64_ZERO(max_byte_size);
    COMPILER_64_ZERO(max_byte_mask);

    COMPILER_64_ZERO(packet64_count);
    COMPILER_64_ZERO(prev_masked_packet64_count);
    COMPILER_64_ZERO(max_packet64_size);
    COMPILER_64_ZERO(max_packet64_mask);

    l_mem = fc->flowcnt_pool[0].mem;
    COMPILER_64_SET(max_byte_size, zero, one);
    COMPILER_64_SHL(max_byte_size,
                    soc_mem_field_length(unit, l_mem, BYTE_COUNTERf));

    COMPILER_64_SET(max_byte_mask,
                    COMPILER_64_HI(max_byte_size),
                    COMPILER_64_LO(max_byte_size));
    COMPILER_64_SUB_32(max_byte_mask,one);

    COMPILER_64_SET(max_packet64_size, zero, one);
    COMPILER_64_SHL(max_packet64_size,
                    soc_mem_field_length(unit, l_mem, PACKET_COUNTERf));

    COMPILER_64_SET(max_packet64_mask,
                    COMPILER_64_HI(max_packet64_size),
                    COMPILER_64_LO(max_packet64_size));
    COMPILER_64_SUB_32(max_packet64_mask,one);

    for (i = 0; i < fc->num_pools; i ++) {
        l_mem = fc->flowcnt_pool[i].mem;
        if ((mem == -1) || (l_mem == mem)){
            fc_pool = &fc->flowcnt_pool[i];

            mem_index_min = soc_mem_index_min(unit, l_mem);
            mem_index_max = soc_mem_index_max(unit, l_mem);
            /*
             * if counter_idx != -1, retrieve specific
             * counter index from the flex counter memory pool.
             */
            if (counter_idx != -1) {
                if ((counter_idx < mem_index_min) ||
                    (counter_idx > mem_index_max)) {
                    return;
                }
                mem_index_min = mem_index_max = counter_idx;
            }
            /* read all the counters or a specific counter from memory */
            if (counter_idx == -1 ) {
                if (soc_mem_read_range(unit, l_mem, MEM_BLOCK_ANY,
                                       mem_index_min, mem_index_max,
                                       flowcnt_temp_counter[unit])
                                       != BCM_E_NONE) {
                    return;
                }
            } else {
                if (soc_mem_read(unit, l_mem, MEM_BLOCK_ANY,
                                 counter_idx,
                                &flowcnt_temp_counter[unit][counter_idx])
                                 != BCM_E_NONE) {
                    return;
                }
            }
            if (((soc_memacc_init(unit, l_mem, PACKET_COUNTERf, &memacc_pkt))) ||
                ((soc_memacc_init(unit, l_mem, BYTE_COUNTERf, &memacc_byte))) ||
                ((soc_memacc_init(unit, l_mem, COUNT_BYTEf, &memacc_countmode)))) {
                  return;
            }

            for (index = mem_index_min; index <= mem_index_max; index++) {
                count_mode = soc_memacc_field32_get(&memacc_countmode,
                            (uint32 *)&flowcnt_temp_counter[unit][index]);

                if (count_mode) {
                    soc_memacc_field64_get(&memacc_byte,
                            (uint32 *)&flowcnt_temp_counter[unit][index],
                            &byte_count);
                } else {
                    packet_count = soc_memacc_field32_get(&memacc_pkt,
                            (uint32 *)&flowcnt_temp_counter[unit][index]);
                    if (fc_pool->flow_packet_counter[index] != packet_count) {
                        LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                                    (BSL_META_U(unit,
                                                "Pool:%d==>"
                                                 "Old Packet Count Value\t:"
                                                 "Index:%d %x\t"
                                                 "New Packet Count Value %x\n"),
                                     i,
                                     index,
                                     fc_pool->flow_packet_counter[index],
                                     packet_count));
                        fc_pool->flow_packet_counter[index] = packet_count;
                    }
                }

                COMPILER_64_SET(packet64_count,0, packet_count);
                COMPILER_64_SET(prev_masked_packet64_count, 
                    COMPILER_64_HI(fc_pool->flow_packet64_counter[index]),
                    COMPILER_64_LO(fc_pool->flow_packet64_counter[index]));
                COMPILER_64_AND(prev_masked_packet64_count,max_packet64_mask);

                if (COMPILER_64_GT(prev_masked_packet64_count, packet64_count)) {
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "Roll over  happend \n")));
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "...Read Packet64 Count    : %x:%x\n"),
                                COMPILER_64_HI(packet64_count),
                                COMPILER_64_LO(packet64_count)));
                    COMPILER_64_ADD_64(packet64_count,max_packet64_size);
                    COMPILER_64_SUB_64(packet64_count,prev_masked_packet64_count);
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                                (BSL_META_U(unit,
                                            "...Diffed 64-Packet Count    : %x:%x\n"),
                                 COMPILER_64_HI(packet64_count),
                                 COMPILER_64_LO(packet64_count)));
                } else {
                    COMPILER_64_SUB_64(packet64_count,prev_masked_packet64_count);
                }
                /* Add difference (if it is) */
                if (!COMPILER_64_IS_ZERO(packet64_count)) {
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                                (BSL_META_U(unit,
                                            "Pool:%d==>"
                                            "Old 64-Packet Count Value\t"
                                            ":Index:%d %x:%x\t"),
                                 i, index,
                                 COMPILER_64_HI(fc_pool->
                                                flow_packet64_counter[index]),
                                 COMPILER_64_LO(fc_pool->
                                                flow_packet64_counter[index])));
                    COMPILER_64_ADD_64(fc_pool->flow_packet64_counter[index],
                                       packet64_count);
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "New 64-Packet Value : %x:%x\n"),
                                COMPILER_64_HI(fc_pool->
                                               flow_packet64_counter[index]),
                                COMPILER_64_LO(fc_pool->
                                               flow_packet64_counter[index])));
                }
                COMPILER_64_SET(prev_masked_byte_count,
                               COMPILER_64_HI(fc_pool->flow_byte_counter
                                              [index]),
                               COMPILER_64_LO(fc_pool->flow_byte_counter
                                              [index]));
                COMPILER_64_AND(prev_masked_byte_count,max_byte_mask);
                if (COMPILER_64_GT(prev_masked_byte_count, byte_count)) {
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "Roll over  happend \n")));
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "...Read Byte Count    : %x:%x\n"),
                                COMPILER_64_HI(byte_count),
                                COMPILER_64_LO(byte_count)));
                    COMPILER_64_ADD_64(byte_count,max_byte_size);
                    COMPILER_64_SUB_64(byte_count,prev_masked_byte_count);
                    LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "...Diffed Byte Count    : %x:%x\n"),
                                COMPILER_64_HI(byte_count),
                                COMPILER_64_LO(byte_count)));
                } else {
                   COMPILER_64_SUB_64(byte_count,prev_masked_byte_count);
                }
                /* Add difference (if it is) */
                if (!COMPILER_64_IS_ZERO(byte_count)) {
                   LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "Pool:%d==>"
                                            "Old Byte Count Value\t"
                                            ":Index:%d %x:%x\t"),
                                i, index,
                                COMPILER_64_HI(fc_pool->
                                               flow_byte_counter[index]),
                                COMPILER_64_LO(fc_pool->
                                               flow_byte_counter[index])));
                   COMPILER_64_ADD_64(fc_pool->flow_byte_counter[index],
                                      byte_count);
                   LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                               (BSL_META_U(unit,
                                           "New Byte Count Value : %x:%x\n"),
                                COMPILER_64_HI(fc_pool->
                                               flow_byte_counter[index]),
                                COMPILER_64_LO(fc_pool->
                                               flow_byte_counter[index])));
                }
            }
        }
    }
    return;
}


/*
 * Function:
 *      _bcm_esw_flowcnt_callback
 * Description:
 *      Flex Counter threads callback function. It is called periodically
 *      to sync s/w copy with h/w copy.
 * Parameters:
 *      unit                  - (IN) unit number
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
void _bcm_esw_flowcnt_callback(int unit)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;

    (void)_bcm_esw_flowcnt_control_get(unit, &fc);
    if (fc == NULL) {
        return;
    }
    _FLOWCNT_LOCK(fc);
    _bcm_esw_flowcnt_counter_collect(unit, -1, -1);
    _FLOWCNT_UNLOCK(fc);

    return;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_cleanup
 * Description:
 *      Clean and free all allocated flow counter resources
 * Parameters:
 *      unit                  - (IN) unit number
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flowcnt_cleanup(int unit)
{
    _bcm_esw_flowcnt_control_t  *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    int i;

    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "_bcm_esw_flowcnt_cleanup \n")));
    if (flowcnt_control[unit] != NULL) {
        fc = flowcnt_control[unit];
        if (valid_stat_group_mode_list[unit] != NULL) {
            valid_stat_group_mode_list[unit] = NULL;
        }
        if (valid_stat_object_list[unit] != NULL) {
            valid_stat_object_list[unit] = NULL;
        }
        if (flowcnt_profile_table[unit] != NULL) {
            flowcnt_profile_table[unit] = NULL;
        }
        if (flowcnt_pool_table[unit] != NULL) {
            flowcnt_pool_table[unit] = NULL;
        }
        if (flowcnt_temp_counter[unit] != NULL) {
            _FLOWCNT_FREE(unit, flowcnt_temp_counter[unit], 1);
            flowcnt_temp_counter[unit] = NULL;
        }
        if (local_scache_ptr[unit] != NULL) {
            _FLOWCNT_FREE(unit, local_scache_ptr[unit], 0);
            local_scache_ptr[unit] = NULL;
        }
        for (i = 0; i < fc->num_profiles; i++) {
            if (fc->default_attr_mode_id[i] != NULL) {
                _FLOWCNT_FREE(unit, fc->default_attr_mode_id[i], 0);
            }
        }

        if(fc->flowcnt_profile != NULL) {
            for (i = 0; i < fc->num_profiles; i++) {
                fc_profile = &fc->flowcnt_profile[i];
                if (fc_profile->flowcnt_group != NULL) {
                    _FLOWCNT_FREE(unit, fc_profile->flowcnt_group, 0);
                    fc_profile->flowcnt_group = NULL;
                }
                if (fc_profile->profile_mem.tables != NULL) {
                    soc_profile_mem_destroy(unit, &fc_profile->profile_mem);
                }
                sal_memset(fc_profile, 0, sizeof(_bcm_esw_flowcnt_profile_t));
            }
            _FLOWCNT_FREE(unit, fc->flowcnt_profile, 0);
            fc->flowcnt_profile = NULL;
        }

        if(fc->flowcnt_pool != NULL) {
            for (i = 0; i < fc->num_pools; i++) {
                fc_pool = &fc->flowcnt_pool[i];
                if (fc_pool->flow_byte_counter != NULL) {
                    _FLOWCNT_FREE(unit, fc_pool->flow_byte_counter, 1);
                    fc_pool->flow_byte_counter = NULL;
                }
                if (fc_pool->flow_packet64_counter != NULL) {
                    _FLOWCNT_FREE(unit, fc_pool->flow_packet64_counter, 1);
                    fc_pool->flow_packet64_counter = NULL;
                }
                if (fc_pool->flow_packet_counter != NULL) {
                    _FLOWCNT_FREE(unit, fc_pool->flow_packet_counter, 1);
                    fc_pool->flow_packet_counter = NULL;
                }
                if (fc_pool->flow_base_index_reference_count != NULL) {
                    _FLOWCNT_FREE(unit,
                                  fc_pool->flow_base_index_reference_count,
                                  0);
                    fc_pool->flow_base_index_reference_count = NULL;
                }
                if (fc_pool->associate_profile_group != NULL) {
                    _FLOWCNT_FREE(unit,
                                  fc_pool->associate_profile_group,
                                  0);
                    fc_pool->associate_profile_group = NULL;
                }
                if (fc_pool->object != NULL) {
                    _FLOWCNT_FREE(unit,
                                  fc_pool->object,
                                  0);
                    fc_pool->object = NULL;
                }
                if (fc_pool->flow_stat_type != NULL) {
                    _FLOWCNT_FREE(unit, fc_pool->flow_stat_type, 0);
                    fc_pool->flow_stat_type = NULL;
                }
                if (fc_pool->flowcnt_aidxres_list_handle != 0) {
                    shr_aidxres_list_destroy(fc_pool->flowcnt_aidxres_list_handle);
                    fc_pool->flowcnt_aidxres_list_handle = 0;
                }
                sal_memset(fc_pool, 0, sizeof(_bcm_esw_flowcnt_pool_t));
            }
            _FLOWCNT_FREE(unit, fc->flowcnt_pool, 0);
            fc->flowcnt_pool = NULL;
        }
        soc_counter_extra_unregister(unit, _bcm_esw_flowcnt_callback);
        if (fc->flowcnt_mutex != NULL) {
            sal_mutex_destroy(fc->flowcnt_mutex);
            fc->flowcnt_mutex = NULL;
        }
        _FLOWCNT_FREE(unit, flowcnt_control[unit], 0);
        flowcnt_control[unit] = NULL;
    }

    return BCM_E_NONE;
}

void
_bcm_esw_flowcnt_counter_id_set(
     bcm_stat_group_mode_t group,
     bcm_stat_object_t     object,
     uint32                profile_group,
     uint32                pool_idx,
     uint32                base_idx,
     uint32                *stat_counter_id)
{
     *stat_counter_id = ((profile_group & _BCM_FLOWCNT_MODE_MASK)   <<
                                 _BCM_FLOWCNT_MODE_START_BIT)       |
                        ((group & _BCM_FLOWCNT_GROUP_MASK)          <<
                                 _BCM_FLOWCNT_GROUP_START_BIT)      |
                        ((pool_idx & _BCM_FLOWCNT_POOL_MASK)        <<
                                 _BCM_FLOWCNT_POOL_START_BIT)       |
                        ((object & _BCM_FLOWCNT_OBJECT_MASK)        <<
                                 _BCM_FLOWCNT_OBJECT_START_BIT)     |
                         (base_idx & _BCM_FLOWCNT_BASE_IDX_MASK);
}
void 
_bcm_esw_flowcnt_counter_id_get(
     uint32                stat_counter_id,
     bcm_stat_group_mode_t *group,
     bcm_stat_object_t     *object,
     uint32                *profile_group,
     uint32                *pool_idx,
     uint32                *base_idx)
{
     *profile_group= (uint32 ) ((stat_counter_id >> 
                                              _BCM_FLOWCNT_MODE_START_BIT) &
                                             (_BCM_FLOWCNT_MODE_MASK));
     *group       = (bcm_stat_group_mode_t) ((stat_counter_id >> 
                                              _BCM_FLOWCNT_GROUP_START_BIT) &
                                             (_BCM_FLOWCNT_GROUP_MASK));
     *pool_idx = ((stat_counter_id >> _BCM_FLOWCNT_POOL_START_BIT) &
                                       (_BCM_FLOWCNT_POOL_MASK));
     *object = (bcm_stat_object_t) ((stat_counter_id >>
                                      _BCM_FLOWCNT_OBJECT_START_BIT) &
                                      (_BCM_FLOWCNT_OBJECT_MASK));
     *base_idx    = (stat_counter_id & _BCM_FLOWCNT_BASE_IDX_MASK);
}

/* **********************************************************/
/*              COMPOSITION OF CUSTOM MODE ID               */
/* **********************************************************/
/*              profile_id  8 bit                           */
/*              profile_group_id 8 bits                     */
/*              -----       00     -   00                   */
/*              Reserved profile_id profile_group_id        */
/* **********************************************************/
#define _BCM_FLOWCNT_PROFILE_ID_START_BIT       8
#define _BCM_FLOWCNT_PROFILE_ID_END_BIT        15
#define _BCM_FLOWCNT_PROFILE_GROUP_ID_START_BIT 0
#define _BCM_FLOWCNT_PROFILE_GROUP_ID_END_BIT   7

#define _BCM_FLOWCNT_PROFILE_ID_MASK \
    ((1<<(_BCM_FLOWCNT_PROFILE_ID_END_BIT - _BCM_FLOWCNT_PROFILE_ID_START_BIT+1))-1)
#define _BCM_FLOWCNT_PROFILE_GROUP_ID_MASK \
    ((1<<(_BCM_FLOWCNT_PROFILE_GROUP_ID_END_BIT - _BCM_FLOWCNT_PROFILE_GROUP_ID_START_BIT+1))-1)

static
void _bcm_esw_flowcnt_custom_mode_id_get(
        uint32 mode_id, int *profile_index, int *group_index)
{
    *profile_index = ((mode_id >> _BCM_FLOWCNT_PROFILE_ID_START_BIT) &
                        (_BCM_FLOWCNT_PROFILE_ID_MASK));
    *group_index = mode_id & _BCM_FLOWCNT_PROFILE_GROUP_ID_MASK;
}

static
void _bcm_esw_flowcnt_custom_mode_id_set(
        int profile_index, int group_index, uint32 *mode_id)
{
    *mode_id = ((profile_index & _BCM_FLOWCNT_PROFILE_ID_MASK) <<
                                 _BCM_FLOWCNT_PROFILE_ID_START_BIT) |
                (group_index & _BCM_FLOWCNT_PROFILE_GROUP_ID_MASK);
}

/* 
 * internal fuction : _bcm_esw_flowcnt_selectors_cmp
 * Description: : compare the profile table contents
 * 
 * Parameters:
 *      unit                - (IN) unit number
 *      table               - (IN) profile table
 *      num_entries         - (IN) num entries to be compared
 *      num_src_selectors   - (IN) num of selectores
 *      num_dst_selectors   - (IN) num of selectores
 *      src_attr_selectors  - (IN) src attr selectors
 *      dst_attr_selectors  - (IN) target sttr selectors
 *
 * Return Value:
 *      0: identical content
 */
static
int
_bcm_esw_flowcnt_selectors_cmp(int unit, soc_mem_t table, 
    int num_entries,
    int num_src_selectors,
    int num_dst_selectors,
    _bcm_flowcnt_group_mode_attr_selector_t *src_attr_selectors,
    bcm_stat_group_mode_attr_selector_t *dst_attr_selectors)
{
    uint32 entry_data_size = 0;
    uint32 *s_entries=NULL;
    uint32 *d_entries=NULL;
    int s_entry_index, d_entry_index;
    int i, rv;

    entry_data_size = sizeof(uint32);
    _FLOWCNT_ALLOC(unit, s_entries, uint32, entry_data_size * num_entries,
               "temp selectors table src", 0, rv);
    if (BCM_FAILURE(rv)) {
        return BCM_E_RESOURCE;
    }

    _FLOWCNT_ALLOC(unit, d_entries, uint32, entry_data_size * num_entries,
               "temp selectors table dst", 0, rv); 
    if (BCM_FAILURE(rv)) {
        _FLOWCNT_FREE(unit, s_entries, 0);
        return BCM_E_RESOURCE;
    }

    /* Fill the entries according to attr_selectors*/
    for (i = 0; i < num_src_selectors; i++){
        s_entry_index = src_attr_selectors[i].attr_value;
        if (SOC_MEM_FIELD_VALID(unit, table, OFFSET_VALIDf)){
            soc_mem_field32_set(unit, table, &s_entries[s_entry_index],
                OFFSET_VALIDf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, table, OFFSETf)){
            soc_mem_field32_set(unit, table, &s_entries[s_entry_index],
                OFFSETf, src_attr_selectors[i].counter_offset);
        }
    }
    /* Fill the entries according to attr_selectors*/
    for (i = 0; i < num_dst_selectors; i++){
        d_entry_index = dst_attr_selectors[i].attr_value;
        if (SOC_MEM_FIELD_VALID(unit, table, OFFSET_VALIDf)){
            soc_mem_field32_set(unit, table, &d_entries[d_entry_index],
                OFFSET_VALIDf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, table, OFFSETf)){
            soc_mem_field32_set(unit, table, &d_entries[d_entry_index],
                OFFSETf, dst_attr_selectors[i].counter_offset);
        }
    }

    rv = sal_memcmp(s_entries, d_entries, num_entries*entry_data_size);
    _FLOWCNT_FREE(unit, s_entries, 0);
    _FLOWCNT_FREE(unit, d_entries, 0);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_profile_group_create
 * Description:
 *      Create profile group
 * Parameters:
 *      unit                - (IN) unit number
 *      direction           - (IN) ingress/egress profile table
 *      group_mode          - (IN) group mode from API
 *      total_counters      - (IN) numbers of counter
 *      num_selectors       - (IN) numbers of selectors
 *      attr_selectors      - (IN) attribute selectors 
 *      profile_index       - (OUT) profile_index from API
 *      group_index         - (OUT) group_index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_flowcnt_profile_group_create(
                int unit,
                bcm_stat_flex_direction_t direction,
                bcm_stat_group_mode_t group_mode,
                int total_counters,
                int num_selectors,
                bcm_stat_group_mode_attr_selector_t *attr_selectors,
                int *profile_index,
                int *group_index)

{ 
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    bcm_stat_group_mode_attr_t  attr;
    int i, entry_size=0, entry_index, profile_group;
    int rv = BCM_E_NONE;
    soc_profile_mem_t   profile;
    soc_mem_t target_table = -1;
    int num_group, num_entry;
    uint32 *entries = NULL;


    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    if ((num_selectors !=0) && (attr_selectors == NULL)) {
        return BCM_E_PARAM;
    }
    attr = attr_selectors->attr;

    _FLOWCNT_LOCK(fc);
    for (i = 0; i < fc->num_profiles; i++) {
        fc_profile = &fc->flowcnt_profile[i];
        if ((fc_profile->direction == direction) && 
            (fc_profile->attr == attr)){
            target_table = fc_profile->profile_table;
            *profile_index = i;
            profile = fc_profile->profile_mem;
            num_group = fc_profile->num_groups;
            break;
        }
    }
    if (target_table == -1) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Couldn't find the matched profile.\n")));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_INTERNAL;
    }
    if (total_counters > (1 << fc->counter_block)) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    num_entry = soc_mem_index_count(unit, target_table) / num_group;
    if (num_selectors > num_entry) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                (BSL_META_U(unit,
                            "Couldn't support %d selectors. "
                            "Maximum %d selectors\n"),
                            num_selectors, num_entry));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    /* Allocate the entries */
    entry_size = sizeof(uint32);
    _FLOWCNT_ALLOC(unit, entries, uint32, num_entry * entry_size,
                   "flowcnt offset table entries", 0, rv);

    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }
    _FLOWCNT_UNLOCK(fc);

    /* Fill the entries according to attr_selectors*/
    for (i = 0; i < num_selectors; i++){
        entry_index = attr_selectors[i].attr_value;
        if (entry_index > (num_entry - 1)) {
            _FLOWCNT_FREE(unit, entries, 0);
            return BCM_E_PARAM;
        }
        if (attr_selectors[i].counter_offset > ((1 << fc->counter_block)-1)) {
            _FLOWCNT_FREE(unit, entries, 0);
            return BCM_E_PARAM;
        }
        if (SOC_MEM_FIELD_VALID(unit, target_table, OFFSET_VALIDf)){
            soc_mem_field32_set(unit, target_table, &entries[entry_index],
                OFFSET_VALIDf, 1);
        }
        if (SOC_MEM_FIELD_VALID(unit, target_table, OFFSETf)){
            soc_mem_field32_set(unit, target_table, &entries[entry_index],
                OFFSETf, attr_selectors[i].counter_offset);
        }
    }
    soc_mem_lock(unit, target_table);
    rv = soc_profile_mem_add(unit, &profile,
                             (void *)&entries, num_entry, (uint32 *)group_index);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Error: Adding profile entry"
                               " - %s.\n"), bcm_errmsg(rv)));
        _FLOWCNT_FREE(unit, entries, 0);
        soc_mem_unlock(unit, target_table);
        return (rv);
    }
    profile_group = *group_index / num_entry;
    *group_index = profile_group;
    soc_mem_unlock(unit, target_table);
    _FLOWCNT_FREE(unit, entries, 0);

    /* Update the profile group reference count */
    _FLOWCNT_LOCK(fc);
    fc_profile = &fc->flowcnt_profile[*profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];

    if (fc_pgroup->available == 0) {
        _FLOWCNT_ALLOC(unit, fc_pgroup->attr_selectors,
                       _bcm_flowcnt_group_mode_attr_selector_t,
                       num_selectors * 
                       sizeof(_bcm_flowcnt_group_mode_attr_selector_t),
                       "flowcnt pkt attr per profile group", 0, rv);

        if (BCM_FAILURE(rv)){
            _FLOWCNT_FREE(unit, attr_selectors, 0);
            _FLOWCNT_UNLOCK(fc);
            return rv;
        }
        for (i = 0; i < num_selectors; i++) {
            fc_pgroup->attr_selectors[i].attr =
                attr_selectors[i].attr;
            fc_pgroup->attr_selectors[i].counter_offset =
                attr_selectors[i].counter_offset;
            fc_pgroup->attr_selectors[i].attr_value =
                attr_selectors[i].attr_value;
        }
        fc_pgroup->num_selectors = num_selectors;
        fc_pgroup->total_counters = total_counters;
        fc_pgroup->group_mode = group_mode;
        fc_pgroup->available = 1;
    }
    _FLOWCNT_UNLOCK(fc);

    return rv;
}

int
_bcm_esw_flowcnt_default_attr_config(int unit,
            bcm_stat_group_mode_attr_t attr,
            int num_selctors, 
            bcm_stat_group_mode_attr_selector_t *attr_selectors)
{
    int i;
    for (i = 0; i < num_selctors; i++){
        attr_selectors[i].attr = attr;
        attr_selectors[i].attr_value = i;
        attr_selectors[i].counter_offset = 0;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_object_table_map
 * Description:
 *      Get the object table informantion based on the the giving object
 * Parameters:
 *      unit                - (IN) unit number
 *      object              - (IN) object. 
 *      profile_index       - (OUT) profile_index
 *      pool_index          - (OUT) pool_index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_esw_flowcnt_object_table_map(int unit,
    bcm_stat_object_t object,
    int *profile_index,
    int *pool_index)
{
    int i;
    int object_index = -1;
    soc_mem_t   pool_table, offset_table;
    _bcm_esw_flowcnt_control_t *fc = NULL;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    *pool_index = -1;
    *profile_index = -1;

    for (i = 0; valid_stat_object_list[unit][i].object != -1; i++) {
        if (valid_stat_object_list[unit][i].object == object) {
            object_index = i;
            break;
        }
    }

    if (object_index == -1) {
        return BCM_E_INTERNAL;
    }
    pool_table = valid_stat_object_list[unit][object_index].counter_mem;
    offset_table = valid_stat_object_list[unit][object_index].offset_mem;

    for (i = 0; i < fc->num_profiles; i ++) {
        if (flowcnt_profile_table[unit][i].mem == offset_table) {
            *profile_index = i;
            break;
        }
    }
    if (*profile_index == -1) {
        return BCM_E_INTERNAL;
    }
    for (i = 0; i < fc->num_pools; i ++) {
        if (flowcnt_pool_table[unit][i].pool_mem == pool_table) {
            *pool_index = i;
            break;
        }
    }
    if (*pool_index == -1) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_destroy
 * Description:
 *      Destroy accounting table's statistics completely
 * Parameters:
 *      unit                  - (IN) unit number
 *      object                - (IN) Accounting Object
 *      profile_group         - (IN) profile group for Accounting Object
 *      base_idx              - (IN) Base Index for Accounting Object
 *      pool_number           - (IN) Pool Number for Accounting Object
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_flowcnt_counter_destroy(
            int                  unit,
            bcm_stat_object_t    object,
            uint32               profile_group,
            uint32               base_idx,
            uint32               pool_number)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int pool_idx, profile_index;
    int total_counters;
    int rv, i;
    uint32 free_count=0;
    uint32 alloc_count=0;
    uint32 largest_free=0, stat_counter_id=0;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index,
                                           &pool_idx);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    if (pool_idx != pool_number){
        return BCM_E_PARAM;
    }

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_idx];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];

    if (fc_pgroup->available == 0) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "The profile_group is not available.\n")));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }
    if (shr_aidxres_list_elem_state(fc_pool->flowcnt_aidxres_list_handle,
                                    base_idx) != BCM_E_EXISTS) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Wrong base index %u \n"),
                   base_idx));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }

    total_counters = fc_pgroup->total_counters;
    rv = shr_aidxres_list_free(fc_pool->flowcnt_aidxres_list_handle, base_idx);
    if (BCM_SUCCESS(rv)){
        fc_pool->pool_stat.used_entries -= total_counters;
        for (i = 0; i < total_counters; i ++){
            fc_pool->flow_stat_type[base_idx+i] = 0;
        }
        fc_pool->associate_profile_group[base_idx] = 0;
        fc_pool->object[base_idx] = 0;
        shr_aidxres_list_state(fc_pool->flowcnt_aidxres_list_handle,
            NULL,NULL,NULL,NULL,&free_count,&alloc_count,&largest_free,
            NULL);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                             "Current Pool status free_count:%d alloc_count:%d "
                              "largest_free:%d "
                              "used_entries:%d\n"),
                  free_count,alloc_count,largest_free,
                  fc_pool->pool_stat.used_entries));

        fc_pgroup->reference_count--;
        _bcm_esw_flowcnt_counter_id_set(
                fc_pgroup->group_mode,
                object,
                profile_group,
                pool_idx,
                base_idx,
                &stat_counter_id);
        if (_bcm_esw_flowcnt_stat_id_delete(
                local_scache_ptr[unit], stat_counter_id) != BCM_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                        (BSL_META_U(unit,
                                    "Could not delete entry in scache memory.\n")));
        }
    }
    _FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_create
 * Description:
 *      Allocate counter based on pool_index basis
 * Parameters:
 *      unit                - (IN) unit number
 *      object              - (IN) object.
 *      total_counters      - (IN) Total Counters need to be allocated.
 *      base_index          - (OUT) profile_index
 *      pool_index          - (OUT) pool_index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */ 
int
_bcm_esw_flowcnt_counter_create(int unit, bcm_stat_object_t object,
                                uint32 profile_group,
                                int total_counters, int *base_index,
                                int *pool_index)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    int rv, base_idx_l;
    int pool_idx, profile_index, i;
    uint32 free_count=0;
    uint32 alloc_count=0;
    uint32 largest_free=0;
    uint32 stat_counter_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index,
                                           &pool_idx);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    _FLOWCNT_LOCK(fc);
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    fc_pool = &fc->flowcnt_pool[pool_idx];

    if (fc_pgroup->available == 0) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "The profile_group is not available.\n")));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    rv = shr_aidxres_list_alloc_block(fc_pool->flowcnt_aidxres_list_handle,
         total_counters, (uint32 *)&base_idx_l);

    if (BCM_SUCCESS(rv)){
       LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                             "Allocated  counter for Table:%s "
                              "pool_idx:%d base_idx:%d \n"),
                  SOC_MEM_UFNAME(unit, fc_pool->mem),
                  pool_idx, base_idx_l));

        fc_pool->pool_stat.used_entries += total_counters;
        for (i = 0; i < total_counters; i ++) {
            fc_pool->flow_stat_type[base_idx_l+i] = bcmFlowcntStatTypePacket;
        }
        fc_pool->associate_profile_group[base_idx_l] = 
                    profile_group + _BCM_FLOWCNT_PGROUP_BIAS;
        fc_pool->object[base_idx_l] = object;
        shr_aidxres_list_state(fc_pool->flowcnt_aidxres_list_handle,
            NULL,NULL,NULL,NULL,&free_count,&alloc_count,&largest_free,
            NULL);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                             "Current Pool status free_count:%d alloc_count:%d "
                              "largest_free:%d "
                              "used_entries:%d\n"),
                  free_count,alloc_count,largest_free,
                  fc_pool->pool_stat.used_entries));
        *base_index = base_idx_l;
        *pool_index = pool_idx;

        fc_pgroup->reference_count++;
        _bcm_esw_flowcnt_counter_id_set(
                 fc_pgroup->group_mode,
                 object,
                 profile_group,
                 *pool_index,
                 *base_index,
                 &stat_counter_id);
        if (_bcm_esw_flowcnt_stat_id_insert(
                local_scache_ptr[unit],stat_counter_id) != BCM_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                        (BSL_META_U(unit,
                                "Could not add entry in scache memory."
                                "Attach it\n")));
        }
    }
    _FLOWCNT_UNLOCK(fc);

    return rv;
}
/*
 * Function:
 *      _bcm_esw_flowcnt_object_table_detach
 * Description:
 *      Detach i.e. Disable Igresss accounting table's statistics
 * Parameters:
 *      unit                  - (IN) unit number
 *      table         - (IN) Flex Accounting Table
 *      index                 - (IN) Flex Accounting Table's Index
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_object_table_detach(
            int       unit,
            soc_mem_t table,
            uint32    index)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL; 
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    void *entry_data = NULL;
    uint32 entry_data_size = 0;
    soc_mem_info_t *memp = NULL;
    int i, valid = -1;
    uint32 profile_ptr, base_index;
    flowcnt_object_info_t *stat_object_list;
    int object_index, pool_index, profile_index;
    counter_table_entry_t counter_entry;
    int rv;
    bcm_stat_object_t object;
    uint32 stat_counter_id;

    stat_object_list = valid_stat_object_list[unit];
    for (i = 0; stat_object_list[i].object_mem != -1; i++) {
        if (table == stat_object_list[i].object_mem) {
            object = stat_object_list[i].object;
            valid = 1;
            break;
        }
    }
    if (valid == -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_accounting_object_info_get(
                            unit, table, index,
                            &profile_ptr, &base_index, &object_index));

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_object_table_map(
                            unit, stat_object_list[object_index].object,
                            &profile_index, &pool_index));

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_ptr];
    fc_pool->flow_base_index_reference_count[base_index]--;
    fc_pool->pool_stat.attached_entries -= fc_pgroup->total_counters;
    /* Clear Counter Values */
    if (fc_pool->flow_base_index_reference_count[base_index] == 0) {
        sal_memset(&counter_entry, 0, sizeof(counter_table_entry_t));
        for (i = 0; i < fc_pgroup->total_counters; i ++) {
            COMPILER_64_SET(fc_pool->flow_packet64_counter[base_index + i],
                            0, 0);
            COMPILER_64_SET(fc_pool->flow_byte_counter[base_index + i],
                            0, 0);
            fc_pool->flow_packet_counter[base_index + i] = 0;
            soc_mem_write(unit, fc_pool->mem, MEM_BLOCK_ALL,
                          base_index + i,
                          &counter_entry);
        }
    }
    
    memp = &SOC_MEM_INFO(unit, table);
    entry_data_size = WORDS2BYTES(BYTES2WORDS(memp->bytes));
    _FLOWCNT_ALLOC(unit, entry_data, void, entry_data_size,
               "object table entry", 0, rv);

    rv = soc_mem_read(unit, table, MEM_BLOCK_ANY,
                      index, entry_data);
    if (BCM_FAILURE(rv)) {
        _FLOWCNT_FREE(unit, entry_data, 0);
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    rv = _bcm_esw_flowcnt_object_table_fields_values_set(
            unit, index, table, entry_data, 0, 0);

    if (BCM_FAILURE(rv)) {
        _FLOWCNT_FREE(unit, entry_data, 0);
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    if (soc_mem_write(unit, table, MEM_BLOCK_ALL,
                  index,
                  entry_data) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Table:%s:Index:%d: encounter some problem \n"),
                   SOC_MEM_UFNAME(unit, table), index));
        _FLOWCNT_FREE(unit, entry_data, 0);
        _FLOWCNT_UNLOCK(fc);
        return    BCM_E_INTERNAL;
    }
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "Deallocated Table:%s:Index:%d:mode:%d"
                           "reference_count %d \n"),
               SOC_MEM_UFNAME(unit, table),index,profile_ptr,
               fc_pool->flow_base_index_reference_count[base_index]));

    _FLOWCNT_FREE(unit, entry_data, 0);

    _bcm_esw_flowcnt_counter_id_set(
                 fc_pgroup->group_mode,
                 object,
                 profile_ptr,
                 pool_index,
                 base_index,
                 &stat_counter_id);
    if (fc_pool->flow_base_index_reference_count[base_index] == 0) {
        if (_bcm_esw_flowcnt_stat_id_insert(
                local_scache_ptr[unit],stat_counter_id) != BCM_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                            (BSL_META_U(unit,
                                        "Could not add entry in scache memory.\n")));
        }
    }
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_object_table_attach
 * Description:
 *      Atach i.e. Enable Ingresss accounting table's statistics
 * Parameters:
 *      unit                  - (IN) unit number
 *      table         - (IN) Flex Accounting Table
 *      index                 - (IN) Flex Accounting Table's Index
 *      mode                  - (IN) Flex offset mode for Accounting Object
 *      base_idx              - (IN) Flex Base Index for Accounting Object
 *      pool_idx              - (IN) Flex Pool Index for Accounting Object
 *      ingress_entry_data1   - (IN) Entry Data(Null or Valid)
 *                                   Null, table will be modified
 *                                   NON-Null, table won't be modified
 * Return Value:
 *      BCM_E_XXX
 */
int _bcm_esw_flowcnt_object_table_attach(
            int                 unit,
            bcm_stat_object_t   object,
            soc_mem_t           table,
            uint32              index,
            int                 profile_group,
            uint32              base_idx,
            uint32              pool_idx,
            void                *entry_data1)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int rv;
    void    *entry_data_temp = NULL;
    void    *entry_data = NULL;
    uint32  entry_data_size = 0;
    soc_mem_info_t *memp = NULL;
    uint32 profile_ptr, base_index;
    int object_table_index, pool_idx_l, profile_index;
    uint32 stat_counter_id;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index, &pool_idx_l);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (pool_idx_l != pool_idx) {
        return BCM_E_INTERNAL;
    }

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_idx];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];

    if (fc_pgroup->available == 0) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "flowcnt profile group:%d:Not configured yet\n"),
                   profile_group));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }
    if (shr_aidxres_list_elem_state(fc_pool->flowcnt_aidxres_list_handle,
                                    base_idx) != BCM_E_EXISTS) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Wrong base index %u \n"),
                   base_idx));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }
    _FLOWCNT_UNLOCK(fc);

    memp = &SOC_MEM_INFO(unit, table);
    entry_data_size = WORDS2BYTES(BYTES2WORDS(memp->bytes));

    if (entry_data1  == NULL ) {
        _FLOWCNT_ALLOC(unit, entry_data, void, entry_data_size,
                   "object table entry", 0, rv);
        if (BCM_FAILURE(rv)){
            return rv;
        }
        if (soc_mem_read(unit, table, MEM_BLOCK_ANY,
                         index,
                         entry_data) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Read failure for Table %s with index %d \n"),
                       SOC_MEM_UFNAME(unit, table),index));
            _FLOWCNT_FREE(unit, entry_data, 0);
            return    BCM_E_INTERNAL;
        }
        entry_data_temp = entry_data;
    } else {
        entry_data_temp = entry_data1;
    }

    if (soc_mem_field_valid(unit, table, VALIDf)) {
        if (soc_mem_field32_get(unit, table, entry_data_temp,
            VALIDf) == 0) {
            LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Table %s  with index %d is Not valid \n"),
                       SOC_MEM_UFNAME(unit, table),index));
            if (entry_data1 == NULL) {
                _FLOWCNT_FREE(unit, entry_data, 0);
            }
            return BCM_E_PARAM;
        }
    }
    rv = _bcm_esw_flowcnt_object_table_fields_values_get(
            unit, index, table, entry_data_temp, 
            &profile_ptr, &base_index);
    if (BCM_FAILURE(rv)) {
        if (entry_data1 == NULL) {
            _FLOWCNT_FREE(unit, entry_data, 0);
        }
        return rv;
    }
    if (base_index != 0) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Table:%s Has already allocated with index:%d"
                               "base %d mode %d."
                               "First dealloc it \n"), 
                   SOC_MEM_UFNAME(unit, table),
                   index,base_index,profile_ptr));
        if (entry_data1 == NULL) {
            _FLOWCNT_FREE(unit, entry_data, 0);
        }
        return BCM_E_EXISTS;/*Either NotConfigured or deallocated before */
    }
    rv = _bcm_esw_flowcnt_object_table_fields_values_set(
            unit, index, table, entry_data_temp, profile_group, base_idx);
    if (BCM_FAILURE(rv)) {
        if (entry_data1 == NULL) {
            _FLOWCNT_FREE(unit, entry_data, 0);
        }
        return rv;
    }
    if (entry_data1 == NULL) {
        if (soc_mem_write(unit, table, MEM_BLOCK_ALL,
                          index,
                          entry_data_temp) != SOC_E_NONE) {
            _FLOWCNT_FREE(unit, entry_data, 0);
            return    BCM_E_INTERNAL;
        }
    }

    rv = _bcm_esw_flowcnt_object_table_index_get(
        unit, index, table, entry_data_temp, &object_table_index);
    if (BCM_FAILURE(rv)) {
        if (entry_data1 == NULL) {
            _FLOWCNT_FREE(unit, entry_data, 0);
        }
        return rv;
    }

    if (entry_data1 == NULL) {
        _FLOWCNT_FREE(unit, entry_data, 0);
    }

    _FLOWCNT_LOCK(fc);
    _bcm_esw_flowcnt_counter_id_set(
                 fc_pgroup->group_mode, 
                 object,
                 profile_group,
                 pool_idx,
                 base_idx,
                 &stat_counter_id);
    if (fc_pool->flow_base_index_reference_count[base_idx] == 0) {
        if (_bcm_esw_flowcnt_stat_id_delete(
                local_scache_ptr[unit], stat_counter_id) != BCM_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                            (BSL_META_U(unit,
                                        "Could not delete entry in scache memory.\n")));
        }
    }  
    fc_pool->flow_base_index_reference_count[base_idx]++;
    fc_pool->pool_stat.attached_entries += fc_pgroup->total_counters;
    _FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_init
 * Description:
 *      Initialize and allocate all required resources
 * Parameters:
 *      unit                  - (IN) unit number
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_esw_flowcnt_init(int unit)
{
    int rv=BCM_E_NONE;
    _bcm_esw_flowcnt_control_t  *fc = NULL;
#ifdef BCM_WARM_BOOT_SUPPORT
    int stable_size=0;
    int stable_used_size=0;
    int additional_scache_size = 0;
#endif

    /* clean up the resource */
    if (flowcnt_control[unit] != NULL) {
        rv = _bcm_esw_flowcnt_cleanup(unit);
        if (BCM_FAILURE(rv)){
            return rv;
        }
    }

    /* Allocate Flowcnt control memeory for this unit. */
    _FLOWCNT_ALLOC(unit, fc, _bcm_esw_flowcnt_control_t,
                   sizeof(_bcm_esw_flowcnt_control_t),
                   "Flowcnt control", 0, rv);
    if (BCM_FAILURE(rv)){
        return rv;
    }

    /* Set up the unit flowcnt control structure. */
    flowcnt_control[unit] = fc;

    /* Create protection mutex. */
    fc->flowcnt_mutex = sal_mutex_create("flowcnt_control.lock");
    if (NULL == fc->flowcnt_mutex) {
        _bcm_esw_flowcnt_cleanup(unit);
        return (BCM_E_INTERNAL);
    }
    /* 
     * chip specific table assignment 
     * NOTE: chip specific information need to be addressed here.
     */
    if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_hr3_flowcnt_ctrl_init(unit, fc);
        if (BCM_FAILURE(rv)) {
            _bcm_esw_flowcnt_cleanup(unit);
            return (BCM_E_MEMORY);
        }
    }
    /* common flowcnt ctrl init */
    rv = _bcm_flowcnt_ctrl_common_init(unit, fc);
    if (BCM_FAILURE(rv)) {
        _bcm_esw_flowcnt_cleanup(unit);
        return (BCM_E_MEMORY);
    }

    rv = soc_counter_extra_register(unit, _bcm_esw_flowcnt_callback);
    if (BCM_FAILURE(rv)) {
        _bcm_esw_flowcnt_cleanup(unit);
        return (BCM_E_MEMORY);
    }

    local_scache_size = FLOWCNT_SCACHE_SIZE_1;
    _FLOWCNT_ALLOC(unit, local_scache_ptr[unit], uint8, 
                   local_scache_size,
                   "local scahced", 0, rv);
    if (BCM_FAILURE(rv)) {
        _bcm_esw_flowcnt_cleanup(unit);
        return (BCM_E_MEMORY);
    }

    /* WARMBOOT related */
#ifdef BCM_WARM_BOOT_SUPPORT
    flowcnt_scache_allocated_size[unit] = 0;
    /* flowcnt will not co-exist with flex cnt, use the same partition */
    SOC_SCACHE_HANDLE_SET(handle, unit, BCM_MODULE_STAT,
                          _BCM_STAT_WARM_BOOT_CHUNK_FLEX);
    SOC_SCACHE_MODULE_MAX_PARTITIONS_SET(unit, BCM_MODULE_STAT, 1);
    local_scache_size += FLOWCNT_SCACHE_SIZE_2_LOCAL;
    local_scache_size += FLOWCNT_SCACHE_SIZE_3;
    if (!SOC_WARM_BOOT(unit)) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "ColdBoot: Modified Local Scache Size %d \n"),
                   local_scache_size));
        rv = soc_stable_size_get(unit, &stable_size);
        if (BCM_FAILURE(rv)) {
            _bcm_esw_flowcnt_cleanup(unit);
            return (rv);
        }
        if (stable_size == 0) {
            LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "STABLE size is zero.Probably NotConfigured yet\n")));
            return BCM_E_NONE;
        }

        /* Check if scache already allocated */
        rv = soc_scache_ptr_get(unit,
                                handle,
                                (uint8 **)&flowcnt_scache_ptr[unit],
                                &flowcnt_scache_allocated_size[unit]);
        if (rv == BCM_E_NOT_FOUND) {
            /* We have not allocated scache, continue with allocation */
            rv = soc_stable_used_get(unit, &stable_used_size);
            if (BCM_FAILURE(rv)) {
                _bcm_esw_flowcnt_cleanup(unit);
                return rv;
            }

            if ((stable_size - stable_used_size) < local_scache_size) {
                 LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                           (BSL_META_U(unit,
                                       "Not enough scache memory left...\n")));
                 _bcm_esw_flowcnt_cleanup(unit);
                 return BCM_E_CONFIG;
            }

            rv = soc_scache_alloc(unit, handle, local_scache_size);
            if (!((rv == BCM_E_NONE) || (rv == BCM_E_EXISTS))) {
                 LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                           (BSL_META_U(unit,
                                       "Seems to be some internal "
                                       "problem.\n")));
                 _bcm_esw_flowcnt_cleanup(unit);
                 return rv;
            }
        } else if (rv == SOC_E_NONE) {
            /* We have already allocated scache (reload case) confirmed size */
            if (flowcnt_scache_allocated_size[unit] < local_scache_size) {
                /* scache is increased due to increase in attribute selectors.
                 * So realloc size to accomodate wb sync.
                 */
                additional_scache_size = local_scache_size -
                                         flowcnt_scache_allocated_size[unit];
                rv = soc_scache_realloc(unit, handle,
                                        additional_scache_size);
                if (!((rv == BCM_E_NONE) || (rv == BCM_E_EXISTS))) {
                    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                              (BSL_META_U(unit,
                                          "Seems to be some internal "
                                          "problem.\n")));
                    _bcm_esw_flowcnt_cleanup(unit);
                    return rv;
                }
            }
        } else {
             LOG_WARN(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Seems to be some internal problem.\n")));
             _bcm_esw_flowcnt_cleanup(unit);
             return rv;
        }

        rv = soc_scache_ptr_get(unit,
                                handle,
                                (uint8 **)&flowcnt_scache_ptr[unit],
                                &flowcnt_scache_allocated_size[unit]);
        if (BCM_FAILURE(rv)) {
            LOG_WARN(BSL_LS_BCM_FLEXCTR,
                     (BSL_META_U(unit,
                                 "Error getting scache handle\n")));
            _bcm_esw_flowcnt_cleanup(unit);
            return rv;
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "WARM Booting... \n")));

        rv = soc_scache_ptr_get(unit,
                                handle,
                                (uint8 **)&flowcnt_scache_ptr[unit],
                                &flowcnt_scache_allocated_size[unit]);

        /* You may get BCM_E_NOT_FOUND for level 1 warm boot */
        if (rv == BCM_E_NOT_FOUND) {
             LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                         (BSL_META_U(unit,
                                     "Seems to be Level-1 Warm boot..."
                                     "continuing..\n")));
            _bcm_esw_flowcnt_warmboot_object_retrive(unit);
            return BCM_E_NONE;
        }
        if (rv != BCM_E_NONE) {
             LOG_WARN(BSL_LS_BCM_FLEXCTR,
                      (BSL_META_U(unit,
                                  "Seems to be some internal problem.\n")));
             _bcm_esw_flowcnt_cleanup(unit);
             return rv;
        }
    }
    if (flowcnt_scache_ptr[unit] == NULL) {
        _bcm_esw_flowcnt_cleanup(unit);
        return BCM_E_MEMORY;
    }
    if (flowcnt_scache_allocated_size[unit] != local_scache_size) {
        if (!SOC_WARM_BOOT(unit)) {
            _bcm_esw_flowcnt_cleanup(unit);
            return BCM_E_INTERNAL;
        } else if (flowcnt_scache_allocated_size[unit] > local_scache_size){
            LOG_WARN(BSL_LS_BCM_FLEXCTR,
                     (BSL_META_U(unit,
                      "Seems like the info in scache is larger\
                      (maybe because of firmware downgrade)\n")));
        }
    }
    if (SOC_WARM_BOOT(unit)) {
        sal_memcpy(&local_scache_ptr[unit][0],
                   &flowcnt_scache_ptr[unit][0],
                   FLOWCNT_SCACHE_SIZE_1);
        _bcm_esw_flowcnt_warmboot_info_recover(unit, 
            flowcnt_scache_ptr[unit]);
        _bcm_esw_flowcnt_warmboot_object_retrive(unit);
    }
#else
    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "COLD Booting... \n")));
#endif

    return BCM_E_NONE;
}
/*
 * Function:
 *  _bcm_esw_flowcnt_profile_group_destroy
 * Description:
 *  Destroys profile group resources
 * Parameters:
 *  
 *  unit                : (IN)  UNIT number      
 *  profile_index       : (IN) index to profile table
 *  group_index         : (IN) index to profile group
 *
 * Returns:
 *  BCM_E_*
 */
STATIC int
_bcm_esw_flowcnt_profile_group_destroy(
    int unit,
    int profile_index,
    int group_index)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    bcm_stat_group_mode_attr_t attr;
    int d_profile_index, d_profile_group;
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _FLOWCNT_LOCK(fc);
    if (profile_index >= fc->num_profiles) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    fc_profile = &fc->flowcnt_profile[profile_index];
    if (group_index >= fc_profile->num_groups) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    fc_pgroup = &fc_profile->flowcnt_group[group_index];

    if (fc_pgroup->available == 0) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "flex counter mode %d not configured yet\n"),
                   group_index));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }
    if (fc_pgroup->reference_count != 0) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "FlexCounterMode:%d:IsBeingUsed."
                              "ReferenceCount:%d:\n"),
                   group_index,fc_pgroup->reference_count));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_INTERNAL;
    }
    soc_mem_lock(unit, fc_profile->profile_table);
    /* Reset profile(offset) table */
    rv = soc_profile_mem_delete(unit, &fc_profile->profile_mem, 
                                group_index*fc_profile->num_entries);
    soc_mem_unlock(unit, fc_profile->profile_table);
    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Fail to reset profile group %d\n"),
                   group_index));
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }
    attr = fc_profile->attr;
    d_profile_index = -1;
    d_profile_group = -1;
    if (fc->default_attr_mode_id[profile_index][attr] != 0) {
        _bcm_esw_flowcnt_custom_mode_id_get(
            fc->default_attr_mode_id[profile_index][attr],
            &d_profile_index, &d_profile_group);
        if ((d_profile_index == profile_index) &&
            (d_profile_group == group_index)) {
            fc->default_attr_mode_id[profile_index][attr] = 0;
        }
    }
    fc_pgroup->available = 0;
    _FLOWCNT_FREE(unit, fc_pgroup->attr_selectors, 0);
    sal_memset(fc_pgroup, 0, sizeof(_bcm_esw_flowcnt_profile_group_t));
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}    
/*
 * Function:
 *      _bcm_esw_flowcnt_group_destroy
 * Description:
 *      Release HW counter resources as per given counter id and makes system
 *      unavailable for any further stat collection action
 * Parameters:
 *      unit            - (IN) unit number
 *      Stat_counter_id - (IN) Stat Counter Id
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_group_destroy(
            int    unit,
            uint32 stat_counter_id)
{    
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    int profile_index, pool_idx;
    int rv = BCM_E_NONE;

    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object,
                                    &profile_group, &pool_index, &base_index);

    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "Deleting : mode:%d group_mode:%d pool:%d object:%d"
                          "base:%d\n stat_counter_id:%d\n"),profile_group,
                          group_mode, pool_index, object, base_index,
                          stat_counter_id));

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_object_table_map(
                            unit, object, &profile_index, &pool_idx));

    if (pool_idx != pool_index) {
        return BCM_E_INTERNAL;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_counter_destroy(
                            unit, object, profile_group, 
                            base_index, pool_index));

    /* clean up the profile_group when no stat_couter_id assocated with */
    /* rv = BCM_E_INTERNAL could be ignored here */
    rv = _bcm_esw_flowcnt_profile_group_destroy(
                            unit, profile_index, profile_group);
    if (rv == BCM_E_INTERNAL) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *  _bcm_esw_flowcnt_group_mode_id_destroy
 * Description:
 *  Destroys Customized Group mode
 * Parameters:
 *  
 *  unit                : (IN)  UNIT number
 *  mode_id             : (IN) Created Mode Identifier for Flowcnt Group Mode
 *
 * Returns:
 *  BCM_E_*
 */
int _bcm_esw_flowcnt_group_mode_id_destroy(
    int unit,
    uint32 mode_id)
{
    int profile_id, profile_group_id;

    _bcm_esw_flowcnt_custom_mode_id_get(mode_id, &profile_id, &profile_group_id);

    return _bcm_esw_flowcnt_profile_group_destroy(unit, profile_id, profile_group_id);
}

/*
 * Function:
 *  _bcm_esw_flowcnt_group_mode_id_get
 * Description:
 *  Retrieves Customized Stat Group mode Attributes for given mode_id
 * Parameters:
 *  
 *  unit                : (IN)  UNIT number
 *  mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 *  flags               : (OUT)  STAT_GROUP_MODE_* flags (INGRESS/EGRESS)
 *  total_counters      : (OUT)  Total Counters for Stat Flex Group Mode
 *  num_selectors       : (IN)  Number of Selectors for Stat Flex Group Mode
 *  attr_selectors      : (OUT)  Attribute Selectors for Stat Flex Group Mode
 *  actual_num_selectors: (OUT) Actual Number of Selectors for Stat Flex
 *                              Group Mode
 *
 * Returns:
 *  BCM_E_*
 */
int _bcm_esw_flowcnt_group_mode_id_get(
    int unit,
    uint32 mode_id,
    uint32 *flags,
    uint32 *total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *actual_num_selectors)
{    
    int profile_id, profile_group_id, i;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int effect_num;

    if ((flags == NULL) ||
        (total_counters == NULL) ||
        (actual_num_selectors == NULL)) {
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _bcm_esw_flowcnt_custom_mode_id_get(mode_id, &profile_id, &profile_group_id);

    _FLOWCNT_LOCK(fc);
    if (profile_id >= fc->num_profiles) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    fc_profile = &fc->flowcnt_profile[profile_id];
    if (profile_group_id >= fc_profile->num_groups) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    fc_pgroup = &fc_profile->flowcnt_group[profile_group_id];

    if (fc_pgroup->available == 0) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }

    *flags = fc_pgroup->flags;
    *total_counters = fc_pgroup->total_counters;
    *actual_num_selectors = fc_pgroup->num_selectors;

    if (num_selectors <= *actual_num_selectors) {
        effect_num = num_selectors;
    } else {
        effect_num = *actual_num_selectors;
    }
    for (i = 0 ; i < effect_num; i++) {
        sal_memset(&(attr_selectors[i]), 0,
                   sizeof(bcm_stat_group_mode_attr_selector_t));
        attr_selectors[i].attr
            = fc_pgroup->attr_selectors[i].attr;
        attr_selectors[i].counter_offset
            = fc_pgroup->attr_selectors[i].counter_offset;
        attr_selectors[i].attr_value
            = fc_pgroup->attr_selectors[i].attr_value;
    }
    _FLOWCNT_UNLOCK(fc);

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_esw_flowcnt_group_mode_id_create
 * Description:
 *  Create Customized Stat Group mode for given Counter Attributes
 * Parameters:
 *  
 *  unit           : (IN)  UNIT number      
 *  flags          : (IN)  STAT_GROUP_MODE_* flags (INGRESS/EGRESS)
 *  total_counters : (IN)  Total Counters for Stat Flex Group Mode
 *  num_selectors  : (IN)  Number of Selectors for Stat Flex Group Mode
 *  attr_selectors : (IN)  Attribute Selectors for Stat Flex Group Mode
 *  mode_id        : (OUT) Created Mode Identifier for Stat Flex Group Mode
 *
 * Returns:
 *  BCM_E_*
 */
int 
_bcm_esw_flowcnt_group_mode_id_create(
    int unit,
    uint32 flags,
    uint32 total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *mode_id)
{
    int i, j;
    bcm_stat_group_mode_attr_t attr;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int direction;
    int rv;
    int profile_group, profile_index;

    /* Perform Sanity Checks */
    if (!((flags & BCM_STAT_GROUP_MODE_INGRESS) ||
          (flags & BCM_STAT_GROUP_MODE_EGRESS))) {
         return BCM_E_PARAM;
    }

    if (total_counters == 0) {
         return BCM_E_PARAM;
    }
    attr = -1;
    for (i = 0; i < num_selectors; i ++) {
        if (i == 0) {
            attr = attr_selectors[i].attr;
        } else if (attr != attr_selectors[i].attr) {
            /* in current HR3 flowcnt, each mode has the same attribute */
            return BCM_E_PARAM;
        }
    }

    if (flags & BCM_STAT_GROUP_MODE_INGRESS) {
        direction = bcmStatFlexDirectionIngress;
    } else {
        direction = bcmStatFlexDirectionEgress;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    _FLOWCNT_LOCK(fc);

    /* Checck for existing mode */
    for (i = 0; i < fc->num_profiles; i ++) {
        fc_profile = &fc->flowcnt_profile[i];
        if ((fc_profile->direction == direction) &&
            (fc_profile->attr == attr)) {
            for (j = 0; j < fc_profile->num_groups; j++) {
                fc_pgroup = &fc_profile->flowcnt_group[j];
                if (fc_pgroup->available) {
                    if (_bcm_esw_flowcnt_selectors_cmp (
                        unit, fc_profile->profile_table,
                        fc_profile->num_entries,
                        fc_pgroup->num_selectors,
                        num_selectors,
                        fc_pgroup->attr_selectors,
                        attr_selectors) == 0){
                        LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                                    (BSL_META_U(unit,
                                                "Mode exist \n")));
                        _bcm_esw_flowcnt_custom_mode_id_set(
                            i, j, mode_id);
                        _FLOWCNT_UNLOCK(fc);
                        return BCM_E_EXISTS;
                    }
                }
            }
        }
    }
    /* 
     * Create and add the profile group
     * profile table(HW)would be configured
     */
    rv = _bcm_esw_flowcnt_profile_group_create(unit,
                                               direction,
                                               0,
                                               total_counters,
                                               num_selectors,
                                               attr_selectors,
                                               &profile_index,
                                               &profile_group);

    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    fc_pgroup->flags = flags;

    _bcm_esw_flowcnt_custom_mode_id_set(profile_index, profile_group, mode_id);

    _FLOWCNT_UNLOCK(fc);

    return BCM_E_NONE;
}

/*
 * Function:
 *  _bcm_esw_flowcnt_custom_group_create
 * Description:
 *  Associate an accounting object to customized group mode 
 *
 * Parameters:
 *  
 *  unit                : (IN)  UNIT number
 *  mode_id             : (IN) Created Mode Identifier for Stat Flex Group Mode
 *  object              : (IN) Accounting object
 *  Stat_counter_id     : (OUT) Stat Counter Id
 *  num_entries         : (OUT) Number of Counter entries created
 *
 * Returns:
 *  BCM_E_*
 */
int _bcm_esw_flowcnt_custom_group_create(
    int unit,
    uint32 mode_id,
    bcm_stat_object_t object,
    uint32 *stat_counter_id,
    uint32 *num_entries)
{
    bcm_stat_group_mode_t group_mode= bcmStatGroupModeSingle;
    uint32 total_counters=0;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int profile_id, profile_group_id; 
    int pool_index, base_index;
    int rv;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_stat_object_validate(unit, object));
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _bcm_esw_flowcnt_custom_mode_id_get(mode_id, &profile_id, &profile_group_id);

    _FLOWCNT_LOCK(fc);
    if (profile_id >= fc->num_profiles) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    fc_profile = &fc->flowcnt_profile[profile_id];
    if (profile_group_id >= fc_profile->num_groups) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }    
    fc_pgroup = &fc_profile->flowcnt_group[profile_group_id];
    total_counters = fc_pgroup->total_counters;
    group_mode = bcmFlowcntProfileGroup1 + profile_group_id;
    fc_pgroup->group_mode = group_mode;
    _FLOWCNT_UNLOCK(fc);

    /* Reserve the counter table */
    rv = _bcm_esw_flowcnt_counter_create(unit, object, profile_group_id,
                                         total_counters, 
                                         &base_index, &pool_index);
   if (BCM_FAILURE(rv)){
        return rv;
    }

   _bcm_esw_flowcnt_counter_id_set(
                 group_mode, object,
                 profile_group_id,
                 pool_index,
                 base_index,
                 stat_counter_id);
    *num_entries = total_counters;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_group_create
 * Description:
 *      Reserve HW counter resources as per given group mode and acounting
 *      object and make system ready for further stat collection action
 *
 * Parameters:
 *    Unit            (IN)  Unit number
 *    object          (IN)  Accounting Object
 *    Group_mode      (IN)  Group Mode
 *    Stat_counter_id (OUT) Stat Counter Id
 *    num_entries     (OUT) Number of Counter entries created
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_group_create (
            int                   unit,
            bcm_stat_object_t     object,
            bcm_stat_group_mode_t group_mode,
            uint32                *stat_counter_id,
            uint32                *num_entries)
{
    int total_counters=0;
    int num_selectors;
    bcm_stat_group_mode_attr_selector_t *attr_selectors = NULL;
    bcm_stat_group_mode_attr_t attr;
    bcm_stat_flex_direction_t direction = bcmStatFlexDirectionIngress;
    int profile_group, profile_index;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    int rv = BCM_E_NONE;
    int base_index, pool_index;
    uint32 default_attr_mode_id = 0;


    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_stat_group_mode_validate(unit, group_mode));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_stat_object_validate(unit, object));

    switch (group_mode) {
        case bcmStatGroupModeSingle:
            total_counters = 1;
            break;
        default:
            return BCM_E_PARAM;
    }
    switch (object) {
        case bcmStatObjectIngVlanXlate:
        case bcmStatObjectIngFieldStageLookup:
        case bcmStatObjectEgrVlanXlate:
            num_selectors = 16;
            attr = bcmStatGroupModeAttrPktCfiPri;
            if ((int)object == bcmStatObjectEgrVlanXlate) {
                direction = bcmStatFlexDirectionEgress;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    _FLOWCNT_LOCK(fc);
    if (fc->default_attr_mode_id[direction][attr] == 0) {
        _FLOWCNT_ALLOC(unit, attr_selectors,
                       bcm_stat_group_mode_attr_selector_t,
                       num_selectors * 
                       sizeof(bcm_stat_group_mode_attr_selector_t),
                       "flowcnt default pkt attr", 0, rv);
        if (BCM_FAILURE(rv)){
            _FLOWCNT_UNLOCK(fc);
            return rv;
        }
        /* setup the default attr_selectors based on the the attr */
        rv = _bcm_esw_flowcnt_default_attr_config(unit, attr,
                                                   num_selectors,
                                                   attr_selectors);
        if (BCM_FAILURE(rv)){
            _FLOWCNT_UNLOCK(fc);
            _FLOWCNT_FREE(unit, attr_selectors, 0);
            return rv;
        }    
        /* 
         * Create and add the profile group 
         * profile table(HW)would be configured
         */
        rv = _bcm_esw_flowcnt_profile_group_create(unit,
                                                   direction,
                                                   group_mode,
                                                   total_counters,
                                                   num_selectors,
                                                   attr_selectors,
                                                   &profile_index,
                                                   &profile_group);
        if (BCM_FAILURE(rv)){
            _FLOWCNT_UNLOCK(fc);
            _FLOWCNT_FREE(unit, attr_selectors, 0);
            return rv;
        }

        _bcm_esw_flowcnt_custom_mode_id_set(
            profile_index, profile_group,
            &fc->default_attr_mode_id[direction][attr]);
        fc->default_attr_mode_id[direction][attr] |=
            _BCM_FLOWCNT_MODE_ID_VALID;
        _FLOWCNT_FREE(unit, attr_selectors, 0);
    } else {
        default_attr_mode_id = fc->default_attr_mode_id[direction][attr];
        default_attr_mode_id &= ~(_BCM_FLOWCNT_MODE_ID_VALID);
        _bcm_esw_flowcnt_custom_mode_id_get(default_attr_mode_id,
                                            &profile_index, &profile_group);
    }
    /* Reserve the counter table */
    rv = _bcm_esw_flowcnt_counter_create(unit, object, profile_group,
                                         total_counters, 
                                         &base_index, &pool_index);
    
    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    _FLOWCNT_UNLOCK(fc);

    _bcm_esw_flowcnt_counter_id_set(
                 group_mode, object,
                 profile_group,
                 pool_index,
                 base_index,
                 stat_counter_id);
    *num_entries = total_counters;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_set
 * Description:
 *      Set Flex Counter Values
 * Parameters:
 *      unit                  - (IN) unit number
 *      index                 - (IN) Flex Accounting Table Index
 *      table                 - (IN) Flex Accounting Table
 *      byte_flag             - (IN) Byte/Packet Flag
 *      counter               - (IN) Counter Index
 *      values                - (IN) Counter Value
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_counter_set(
            int              unit,
            uint32           index,
            soc_mem_t        table,
            uint32           byte_flag,
            uint32           counter_index,
            bcm_stat_value_t *value)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int pool_index, offset_index, total_entries;
    int profile_index, object_index;
    uint32 base_index, profile_ptr;
    int rv = BCM_E_NONE;
    void *entry_data = NULL;
    uint32 entry_data_size = 0;
    soc_mem_info_t *memp = NULL; 
    bcm_stat_object_t   object; 
    soc_mem_t   counter_mem;
    uint32 max_packet_mask=0;
    uint64 max_byte_mask;
    uint32 hw_val[2];
    
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_accounting_object_info_get(
                            unit, table, index,
                            &profile_ptr, &base_index, &object_index));

    object = valid_stat_object_list[unit][object_index].object;
    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index,
                                           &pool_index);
    if (BCM_FAILURE(rv)){
        return rv;
    }

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_ptr];
    counter_mem = fc_pool->mem;
    total_entries = fc_pgroup->total_counters;
    offset_index = counter_index;

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX. Must be < Total Counters %d \n"),
                   total_entries));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }

    memp = &SOC_MEM_INFO(unit, counter_mem);
    entry_data_size = WORDS2BYTES(BYTES2WORDS(memp->bytes));
    _FLOWCNT_ALLOC(unit, entry_data, void, entry_data_size,
               "counter table entry", 0, rv);
    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }
    if (soc_mem_read(unit,
                     counter_mem,
                     MEM_BLOCK_ANY,
                     base_index+offset_index,
                     entry_data) != BCM_E_NONE){
        _FLOWCNT_UNLOCK(fc);
        _FLOWCNT_FREE(unit, entry_data, 0);
        return BCM_E_INTERNAL;
    }
    COMPILER_64_ZERO(max_byte_mask);
    if (byte_flag == 1) {
        fc_pool->flow_stat_type[base_index+offset_index] =
                    bcmFlowcntStatTypeByte;
        
        /* Mask to possible max values */
        COMPILER_64_AND(value->bytes, max_byte_mask);
        /* Update Soft Copy */
        COMPILER_64_SET(fc_pool->flow_byte_counter[base_index+offset_index],
                        COMPILER_64_HI(value->bytes),
                        COMPILER_64_LO(value->bytes));
        /* Change Read Hw Copy */
        hw_val[0] = COMPILER_64_LO(value->bytes);
        hw_val[1] = COMPILER_64_HI(value->bytes);
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          BYTE_COUNTERf,
                          hw_val);
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          COUNT_BYTEf,
                          &byte_flag);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Byte Count Value\t:TABLE:%sINDEX:%d "
                               "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   index,
                   pool_index,
                   base_index+offset_index,
                   COMPILER_64_HI(value->bytes),
                   COMPILER_64_LO(value->bytes)));
    } else {
        fc_pool->flow_stat_type[base_index+offset_index] =
                    bcmFlowcntStatTypePacket;

        value->packets &= max_packet_mask;
        /* Update Soft Copy */
        fc_pool->flow_packet_counter[base_index+offset_index] = value->packets;

        /* Update Soft Copy */
        COMPILER_64_SET(fc_pool->flow_packet64_counter[base_index+offset_index],0,
                         value->packets);
        /* Change Read Hw Copy */
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          PACKET_COUNTERf,
                          &(value->packets));
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          COUNT_BYTEf,
                          &byte_flag);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet Count Value\t:TABLE:%sINDEX:%d "
                              "(@Pool:%d ActualOffset%d) : %x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   index,
                   pool_index,
                   base_index+offset_index,
                   value->packets));
    }
    /* Update Hw Copy */
    if (soc_mem_write(unit, counter_mem, MEM_BLOCK_ALL,
                      base_index+offset_index,
                      entry_data) != BCM_E_NONE){
        _FLOWCNT_UNLOCK(fc);
        _FLOWCNT_FREE(unit, entry_data, 0);
        return BCM_E_INTERNAL;
    }
    _FLOWCNT_UNLOCK(fc);
    _FLOWCNT_FREE(unit, entry_data, 0);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_get
 * Description:
 *      Get Flex Counter Values
 *      if sync_mode is set, sync the sw accumulated count
 *      with hw count value first, else return sw count.
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      sync_mode             - (IN) hwcount is to be synced to sw count
 *      index                 - (IN) Flex Accounting Table Index
 *      table                 - (IN) Flex Accounting Table
 *      byte_flag             - (IN) Byte/Packet Flag
 *      counter               - (IN) Counter Index
 *      values                - (OUT) Counter Value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_counter_get(
            int              unit,
            int              sync_mode,
            uint32           index,
            soc_mem_t        table,
            uint32           byte_flag,
            uint32           counter_index,
            bcm_stat_value_t *value)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int object_index, pool_index, profile_index, offset_index, total_entries;
    uint32 base_index, profile_ptr;
    int rv = BCM_E_NONE;
    bcm_stat_object_t   object;
    soc_mem_t   counter_mem;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_accounting_object_info_get(
                            unit, table, index,
                            &profile_ptr, &base_index, &object_index));
    
    object = valid_stat_object_list[unit][object_index].object;
    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index,
                                           &pool_index);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    
    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_ptr];
    counter_mem = fc_pool->mem; 
    total_entries = fc_pgroup->total_counters;
    offset_index = counter_index;
    
    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX. Must be < Total Counters %d \n"),
                   total_entries));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }

    if (byte_flag == 1) {
        if (fc_pool->flow_stat_type[base_index+offset_index] !=
            bcmFlowcntStatTypeByte) {
            _FLOWCNT_UNLOCK(fc);
            return BCM_E_PARAM;
        }
    } else {
        if (fc_pool->flow_stat_type[base_index+offset_index] !=
            bcmFlowcntStatTypePacket) {
            _FLOWCNT_UNLOCK(fc);
            return BCM_E_PARAM;
        }
    }

    /* sync the software counter with hardware counter */
    if (sync_mode == 1) {
        _bcm_esw_flowcnt_counter_collect(unit,
                                        counter_mem,
                                        base_index + offset_index);
    }

    bcm_stat_value_t_init(value);
    if (byte_flag == 1) {
        COMPILER_64_SET(value->bytes,
                        COMPILER_64_HI(fc_pool->
                                       flow_byte_counter[base_index+offset_index]),
                        COMPILER_64_LO(fc_pool->
                                       flow_byte_counter[base_index+offset_index]));
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Byte Count Value\t:TABLE:%sINDEX:%d"
                              "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                  SOC_MEM_UFNAME(unit, counter_mem),
                  index,
                  pool_index,
                  base_index+offset_index,
                  COMPILER_64_HI(value->bytes),
                  COMPILER_64_LO(value->bytes)));
    } else {
        value->packets= fc_pool->
                        flow_packet_counter[base_index+offset_index];
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet Count Value\t:TABLE:%sINDEX:%d "
                              "(@Pool:%dActualOffset%d) : %x \n"),
                  SOC_MEM_UFNAME(unit, counter_mem),
                  index,
                  pool_index,
                  base_index+offset_index,
                  value->packets));
        COMPILER_64_SET(value->packets64,
                        COMPILER_64_HI(fc_pool->
                                       flow_packet64_counter
                                       [base_index+offset_index]),
                        COMPILER_64_LO(fc_pool->
                                       flow_packet64_counter
                                       [base_index+offset_index]));
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet64 Count Value\t:TABLE:%sINDEX:%d "
                               "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                  SOC_MEM_UFNAME(unit, counter_mem),
                  index,
                  pool_index,
                  base_index+offset_index,
                  COMPILER_64_HI(value->packets64),
                  COMPILER_64_LO(value->packets64)));
    }
    _FLOWCNT_UNLOCK(fc);

     return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_esw_flowcnt_stat_id_get
 * Description:
 *      Get stat id of the accounting object
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      table                 - (IN) Accounting Table
 *      index                 - (IN) Accounting Table Index
 *      stat_counter_id       - (OUT) Stat Id
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flowcnt_stat_id_get(
            int         unit,
            soc_mem_t   table,
            int         index,
            uint32      *stat_counter_id)
{
    uint32 base_index, profile_ptr;
    bcm_stat_object_t   object;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int object_index, pool_index, profile_index;
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_accounting_object_info_get(
                            unit, table, index,
                            &profile_ptr, &base_index, &object_index));
                            
    object = valid_stat_object_list[unit][object_index].object;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    

    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index, &pool_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    _FLOWCNT_LOCK(fc);

    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_ptr];

    _FLOWCNT_UNLOCK(fc);

   _bcm_esw_flowcnt_counter_id_set(
                 fc_pgroup->group_mode,
                 object,
                 profile_ptr,
                 pool_index,
                 base_index,
                 stat_counter_id);

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_esw_flowcnt_stat_type_validate
 * Description:
 *      Validate the stat type of the accounting table entry
 *
 * Parameters:
 *      unit                  - (IN) unit number
 *      table                 - (IN) Accounting Table
 *      index                 - (IN) Accounting Table Index
 *      flow_stat_type        - (IN) stat type (bcmFlowcntStatTypeByte/Packet)
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_flowcnt_stat_type_validate(
            int         unit,
            soc_mem_t   table,
            int         index,
            _bcm_flowcnt_stat_type_t flow_stat_type)
{
    uint32 base_index, profile_ptr;
    bcm_stat_object_t   object;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    int object_index, pool_index, profile_index;
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_accounting_object_info_get(
                            unit, table, index,
                            &profile_ptr, &base_index, &object_index));
                            
    object = valid_stat_object_list[unit][object_index].object;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    
    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index, &pool_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];

    if (fc_pool->flow_stat_type[base_index] == 0) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_NOT_FOUND;
    }
    if (fc_pool->flow_stat_type[base_index] != flow_stat_type) {
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    _FLOWCNT_UNLOCK(fc);

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_esw_flowcnt_group_dump_info
 * Description:
 *      Dump Useful Info about configured group
 * Parameters:
 *      unit                  - (IN) unit number
 *      all_flag              - (IN) If 1, object and group_mode are ignored
 *      object                - (IN) Flex Accounting object
 *      group                 - (IN) Flex Group Mode
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_group_dump_info(
     int                   unit,
     int                   all_flag,
     bcm_stat_object_t     object,
     bcm_stat_group_mode_t group)
{
    int i;
    uint32 index = 0, stat_counter_id;
    bcm_stat_object_t l_object;
    bcm_stat_group_mode_t group_mode;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    uint32  profile_group, pool_index, base_index;
    int profile_index, pool_idx;
    uint32 total_counters, num_ctr;
    char *oname[] = BCM_STAT_OBJECT;
    uint32 *ptr = (uint32 *)&local_scache_ptr[unit][0];

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _FLOWCNT_LOCK(fc);
    LOG_CLI((BSL_META_U(unit,
                        "Not attached(MAX=%d) Stat counter Id info \n"),
             MAX_FLOWCNT_SCACHE_SIZE));
    for (index = 0; index < MAX_FLOWCNT_SCACHE_SIZE; index++) {
        if (ptr[index] != 0) {
            stat_counter_id = ptr[index];
            _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode,
                                            &l_object,&profile_group,
                                            &pool_index, &base_index); 
            if (_bcm_esw_flowcnt_object_table_map(
                    unit, l_object, &profile_index, &pool_idx) != BCM_E_NONE){
                continue;
            }
            fc_profile = &fc->flowcnt_profile[profile_index];
            fc_pgroup = &fc_profile->flowcnt_group[profile_group];
            fc_pool = &fc->flowcnt_pool[pool_idx];
            total_counters = fc_pgroup->total_counters;
            if (total_counters == 0) {
                continue;
            }
            if ((all_flag == TRUE) ||
                ((l_object == object) && (group_mode == group))) {
                   LOG_CLI((BSL_META_U(unit,
                                       "\tstat_counter_id = %d=0x%x \n"),
                            stat_counter_id, stat_counter_id));
                   LOG_CLI((BSL_META_U(unit,
                                       "\t\t mode:%d profile_mem:%s"
                                       "\n\t\tpool:%d object:%s base:%d"
                                       " total_counters=%d\n"),
                            profile_group,
                            SOC_MEM_UFNAME(unit, fc_profile->profile_table),
                            pool_idx, oname[l_object],
                            base_index, total_counters));
             }
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "Atached Stat counter Id info \n")));
    for (l_object = 0; l_object < bcmStatObjectMaxValue; l_object++){
        if (_bcm_esw_flowcnt_object_table_map(
                unit, l_object, &profile_index, &pool_idx) != BCM_E_NONE){
            continue;
        }
        fc_profile = &fc->flowcnt_profile[profile_index];
        fc_pool = &fc->flowcnt_pool[pool_idx];
        num_ctr = soc_mem_index_count(unit, fc_pool->mem);
        for (i = 0; i < num_ctr; i++) {
            if (fc_pool->flow_base_index_reference_count[i] != 0){
                if (fc_pool->associate_profile_group[i] <
                    _BCM_FLOWCNT_PGROUP_BIAS){
                   continue;
                }
                if (fc_pool->object[i] != l_object) {
                   continue;
                }
                profile_group = fc_pool->associate_profile_group[i] -
                                _BCM_FLOWCNT_PGROUP_BIAS;
                fc_pgroup = &fc_profile->flowcnt_group[profile_group];
                group_mode = fc_pgroup->group_mode;
                _bcm_esw_flowcnt_counter_id_set(
                             group_mode,
                             l_object,
                             profile_group,
                             pool_idx,
                             i,
                             &stat_counter_id); 
                if ((all_flag == TRUE) ||
                    ((l_object == object) && (group_mode == group))) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\tstat counter id %d (0x%x) "
                                        "object=%s base index=%d (0x%x)\n"),
                            stat_counter_id,stat_counter_id,
                            oname[l_object],
                            i, i));
                }
            }
        }
    }
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_raw_set
 * Description:
 *      Set Flex Counter Values
 * Parameters:
 *      unit                  - (IN) unit number
 *      stat_counter_id       - (IN) Stat Counter Id
 *      byte_flag             - (IN) Byte/Packet Flag
 *      counter               - (IN) Counter Index
 *      values                - (IN) Counter Value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes: Must be called carefully.
 *        Preference should be given to _bcm_esw_flowcnt_counter_set()
 */
int _bcm_esw_flowcnt_counter_raw_set(
            int              unit,
            uint32           stat_counter_id,
            uint32           byte_flag,
            uint32           counter_index,
            bcm_stat_value_t *value)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    int profile_index, pool_idx, total_entries, offset_index;
    soc_mem_t   counter_mem;
    soc_mem_info_t *memp = NULL;
    void *entry_data = NULL;
    uint32 entry_data_size = 0;
    int rv = BCM_E_NONE;
    uint32 max_packet_mask=0;
    uint64 max_byte_mask;
    uint32 hw_val[2];

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object,
                                    &profile_group, &pool_index, &base_index);

    if (group_mode >= bcmStatGroupModeCount) {
        /* custom group */
        if ((group_mode - bcmStatGroupModeCount) != profile_group) {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_flowcnt_stat_group_mode_validate(unit, group_mode));
    }
    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_stat_object_validate(unit, object));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_object_table_map(unit, object, &profile_index,
                                          &pool_idx));

    if (pool_idx != pool_index) {
        return BCM_E_INTERNAL;
    }
    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    counter_mem = fc_pool->mem;
    total_entries = fc_pgroup->total_counters;
    offset_index = counter_index;

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX. Must be < Total Counters %d \n"),
                   total_entries));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    memp = &SOC_MEM_INFO(unit, counter_mem);
    entry_data_size = WORDS2BYTES(BYTES2WORDS(memp->bytes));
    _FLOWCNT_ALLOC(unit, entry_data, void, entry_data_size,
               "counter table entry", 0, rv);
    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }
    if (soc_mem_read(unit,
                     counter_mem,
                     MEM_BLOCK_ANY,
                     base_index+offset_index,
                     entry_data) != BCM_E_NONE){
        _FLOWCNT_UNLOCK(fc);
        _FLOWCNT_FREE(unit, entry_data, 0);
        return BCM_E_INTERNAL;
    }
    COMPILER_64_ZERO(max_byte_mask);
    if (byte_flag == 1) {
        fc_pool->flow_stat_type[base_index+offset_index] =
                    bcmFlowcntStatTypeByte;

        /* Mask to possible max values */
        COMPILER_64_AND(value->bytes, max_byte_mask);
        /* Update Soft Copy */
        COMPILER_64_SET(fc_pool->flow_byte_counter[base_index+offset_index],
                        COMPILER_64_HI(value->bytes),
                        COMPILER_64_LO(value->bytes));
        /* Change Read Hw Copy */
        hw_val[0] = COMPILER_64_LO(value->bytes);
        hw_val[1] = COMPILER_64_HI(value->bytes);
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          BYTE_COUNTERf,
                          hw_val);
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          COUNT_BYTEf,
                          &byte_flag);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Byte Count Value\t:TABLE:%s"
                               "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   pool_index,
                   base_index+offset_index,
                   COMPILER_64_HI(value->bytes),
                   COMPILER_64_LO(value->bytes)));
    } else {
        fc_pool->flow_stat_type[base_index+offset_index] =
                    bcmFlowcntStatTypePacket;

        value->packets &= max_packet_mask;
        /* Update Soft Copy */
        fc_pool->flow_packet_counter[base_index+offset_index] = value->packets;

        /* Update Soft Copy */
        COMPILER_64_SET(fc_pool->flow_packet64_counter[base_index+offset_index],0,
                         value->packets);
        /* Change Read Hw Copy */
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          PACKET_COUNTERf,
                          &(value->packets));
        soc_mem_field_set(unit,
                          counter_mem,
                          (uint32 *)entry_data,
                          COUNT_BYTEf,
                          &byte_flag);
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet Count Value\t:TABLE:%s"
                              "(@Pool:%d ActualOffset%d) : %x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   pool_index,
                   base_index+offset_index,
                   value->packets));
    }
    /* Update Hw Copy */
    if (soc_mem_write(unit, counter_mem, MEM_BLOCK_ALL,
                      base_index+offset_index,
                      entry_data) != BCM_E_NONE){
        _FLOWCNT_UNLOCK(fc);
        _FLOWCNT_FREE(unit, entry_data, 0);
        return BCM_E_INTERNAL;
    }
    _FLOWCNT_UNLOCK(fc);
    _FLOWCNT_FREE(unit, entry_data, 0);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_counter_raw_get
 * Description:
 *      Get Flex Counter Values
 * Parameters:
 *      unit                  - (IN) unit number
 *      sync_mode             - (IN) hwcount is to be synced to sw count
 *      stat_counter_id       - (IN) Stat Counter Id
 *      byte_flag             - (IN) Byte/Packet Flag
 *      counter               - (IN) Counter Index
 *      values                - (OUT) Counter Value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes: Must be called carefully.
 *        Preference should be given to _bcm_esw_flowcnt_counter_get()
 */
int _bcm_esw_flowcnt_counter_raw_get(
            int              unit,
            int              sync_mode,
            uint32           stat_counter_id,
            uint32           byte_flag,
            uint32           counter_index,
            bcm_stat_value_t *value)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    int profile_index, pool_idx, total_entries, offset_index;
    soc_mem_t   counter_mem;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object,
                                    &profile_group, &pool_index, &base_index);

    if (group_mode >= bcmStatGroupModeCount) {
        /* custom group */
        if ((group_mode - bcmStatGroupModeCount) != profile_group) {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_flowcnt_stat_group_mode_validate(unit, group_mode));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_stat_object_validate(unit, object));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_object_table_map(unit, object, &profile_index,
                                          &pool_idx));

    if (pool_idx != pool_index) {
        return BCM_E_INTERNAL;
    }

    _FLOWCNT_LOCK(fc);
    fc_pool = &fc->flowcnt_pool[pool_index];
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    counter_mem = fc_pool->mem;
    total_entries = fc_pgroup->total_counters;
    offset_index = counter_index;

    if (offset_index >= total_entries) {
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX. Must be < Total Counters %d \n"),
                   total_entries));
        _FLOWCNT_UNLOCK(fc);
        return BCM_E_PARAM;
    }
    if (byte_flag == 1) {
        if (fc_pool->flow_stat_type[base_index+offset_index] !=
            bcmFlowcntStatTypeByte) {
            _FLOWCNT_UNLOCK(fc);
            return BCM_E_PARAM;
        }
    } else {
        if (fc_pool->flow_stat_type[base_index+offset_index] !=
            bcmFlowcntStatTypePacket) {
            _FLOWCNT_UNLOCK(fc);
            return BCM_E_PARAM;
        }
    }

    /* sync the software counter with hardware counter */
    if (sync_mode == 1) {
        _bcm_esw_flowcnt_counter_collect(unit,
                                         counter_mem,
                                         base_index + offset_index);
    }

    bcm_stat_value_t_init(value);
    if (byte_flag == 1) {
        COMPILER_64_SET(value->bytes,
                        COMPILER_64_HI(fc_pool->
                                       flow_byte_counter
                                       [base_index+offset_index]),
                        COMPILER_64_LO(fc_pool->
                                       flow_byte_counter
                                       [base_index+offset_index]));
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Byte Count Value\t:TABLE:%s "
                              "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   pool_index,
                   base_index+offset_index,
                   COMPILER_64_HI(value->bytes),
                   COMPILER_64_LO(value->bytes)));
    } else {
        value->packets= fc_pool->
                        flow_packet_counter[base_index+offset_index];
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet Count Value\t:TABLE:%s "
                              "(@Pool:%dActualOffset%d) : %x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   pool_index,
                   base_index+offset_index,
                   value->packets));
        COMPILER_64_SET(value->packets64,
                        COMPILER_64_HI(fc_pool->
                                       flow_packet64_counter
                                       [base_index+offset_index]),
                        COMPILER_64_LO(fc_pool->
                                       flow_packet64_counter
                                       [base_index+offset_index]));
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Packet64 Count Value\t:TABLE:%s"
                              "(@Pool:%d ActualOffset%d) : %x:%x \n"),
                   SOC_MEM_UFNAME(unit, counter_mem),
                   pool_index,
                   base_index+offset_index,
                   COMPILER_64_HI(value->packets64),
                   COMPILER_64_LO(value->packets64)));
    }
    _FLOWCNT_UNLOCK(fc);

     return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flowcnt_total_counters_get
 * Description:
 *      Get the total counters per profile group (derived from stat_counter_id)
 * Parameters:
 *      unit                - (IN) unit number
 *      stat_counter_id     - (IN) Stat Counter Id
 *      total_counters      - (OUT) Total Counters allocated
 *
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_esw_flowcnt_total_counters_get(
    int                  unit,
    uint32               stat_counter_id,
    uint32               *total_counters)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32  profile_group, pool_index, base_index;
    int profile_index, pool_idx;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode, &object,
                                    &profile_group, &pool_index, &base_index);

    if (group_mode >= bcmStatGroupModeCount) {
        /* custom group */
        if ((group_mode - bcmStatGroupModeCount) != profile_group) {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_flowcnt_stat_group_mode_validate(unit, group_mode));
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_flowcnt_object_table_map(unit, object, &profile_index,
                                          &pool_idx));
    if (pool_idx != pool_index) {
        return BCM_E_INTERNAL;
    }
    _FLOWCNT_LOCK(fc);
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    *total_counters = fc_pgroup->total_counters;
    _FLOWCNT_UNLOCK(fc);

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_esw_flowcnt_sync
 * Description:
 *      Sync flex s/w copy to h/w copy
 * Parameters:
 *      unit                  - (IN) unit number
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_flowcnt_sync(int unit)
{
    _bcm_flowcnt_warmboot_info_t *flowcnt_wb_info;
    _bcm_flowcnt_warmboot_group_info_t *flowcnt_wb_group_info;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    int i, j, sel;
    int scache_size = 0;
    int scache_group_mode_num_limit; /* the limit num of group_mode in scache */

    LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
              (BSL_META_U(unit,
                          "_bcm_esw_flowcnt_sync \n")));
    if ((handle == 0) ||
        (flowcnt_scache_allocated_size[unit] == 0) ||
        (flowcnt_scache_ptr[unit] == NULL)) {
         LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                   (BSL_META_U(unit,
                               "Scache memory was not allocate in init!! \n")));
         return BCM_E_CONFIG;
    }

    scache_group_mode_num_limit = FLOWCNT_SCACHE_SIZE_2_RECOVER(unit) / sizeof(uint32);
    if (scache_group_mode_num_limit < bcmStatGroupModeAttrMaxValue) {
        /* in firmware upgrade, the number of group mode maybe increase
         * so dafault_id of some group mode cannot be synced into scache
         */
        LOG_WARN(BSL_LS_BCM_FLEXCTR,
                 (BSL_META_U(unit,
                             "Cannot sync dafault_id after mode id %d\n"),
                             scache_group_mode_num_limit));
    }

    scache_size = FLOWCNT_SCACHE_SIZE_1;
    sal_memcpy(&flowcnt_scache_ptr[unit][0], &local_scache_ptr[unit][0],
               scache_size);

    flowcnt_wb_info = (_bcm_flowcnt_warmboot_info_t *)
                      (&flowcnt_scache_ptr[unit][0] +
                       scache_size);
    scache_size += FLOWCNT_SCACHE_SIZE_2_LOCAL;
    flowcnt_wb_group_info = (_bcm_flowcnt_warmboot_group_info_t *)
                            (&flowcnt_scache_ptr[unit][0] +
                             scache_size);

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    _FLOWCNT_LOCK(fc);

    for (i = 0; i < fc->num_profiles; i++ ){
        for (j = 0; j < bcmStatGroupModeAttrMaxValue; j++) {
            if (j >= scache_group_mode_num_limit) {
                break;
            }
            sal_memcpy(&flowcnt_wb_info->default_attr_mode_id[i][j],
                       &fc->default_attr_mode_id[i][j],
                       sizeof(uint32));
        }
    }

    for (i = 0; i < fc->num_profiles; i++ ){
        fc_profile = &fc->flowcnt_profile[i];
        for (j = 0; j < fc_profile->num_groups; j ++) {
            fc_pgroup = &fc_profile->flowcnt_group[j];
            flowcnt_wb_group_info->group_mode = fc_pgroup->group_mode;
            flowcnt_wb_group_info->flags = fc_pgroup->flags;
            flowcnt_wb_group_info->total_counters = fc_pgroup->total_counters;
            flowcnt_wb_group_info->num_selectors = fc_pgroup->num_selectors;
            for (sel = 0; sel < fc_pgroup->num_selectors; sel ++) {
                sal_memcpy(&flowcnt_wb_group_info->attr_selectors[sel],
                           &fc_pgroup->attr_selectors[sel],
                           sizeof(_bcm_flowcnt_group_mode_attr_selector_t));
            }
            flowcnt_wb_group_info++;
        }
    }
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}
/* 
 * Retrive the total counters per profile group from HW(profile table)
 * NOTE: warmboot purpose
 */
STATIC int
_bcm_esw_flowcnt_profile_table_retrieve(
    int unit,
    int profile_index,
    int profile_group)
{
    soc_mem_t profile_table;
    uint32 entry_data_size = 0;
    uint32 *p_entries = NULL;
    int num_entries, total_counters;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    uint32 offset_valid, offset;
    int i, rv;

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));
    _FLOWCNT_LOCK(fc);
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pgroup = &fc_profile->flowcnt_group[profile_group];
    profile_table = fc_profile->profile_table;
    num_entries = fc_profile->num_entries;

    entry_data_size = sizeof(uint32);
    _FLOWCNT_ALLOC(unit, p_entries, uint32, entry_data_size * num_entries,
                   "temp selectors table src", 1, rv);

    rv = soc_mem_read_range(unit, profile_table, MEM_BLOCK_ANY,
                            profile_group * num_entries,
                            (profile_group + 1) * num_entries,
                            p_entries);
    if (BCM_FAILURE(rv)){        
        _FLOWCNT_FREE(unit, p_entries, 1);
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }
    total_counters = 0;
    for (i = 0; i < num_entries; i++ ){
        soc_mem_field_get(unit, profile_table,
                          (uint32 *)&p_entries[i],
                          OFFSET_VALIDf, &offset_valid);
        soc_mem_field_get(unit, profile_table,
                          (uint32 *)&p_entries[i],
                          OFFSETf, &offset);
        if (offset_valid) {
            /* find the max offset */
            if (total_counters < offset) {
                total_counters = offset;
            }
        }
    }
    /* numbers of counters should be offset +1 */
    fc_pgroup-> total_counters = total_counters + 1;
    _FLOWCNT_FREE(unit, p_entries, 1);
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}

/* 
 * Retrive the pool, profile_table info from HW(object table)
 * NOTE: warmboot purpose
 */
STATIC int
_bcm_esw_flowcnt_warmboot_object_table_retrieve(
    int unit,
    bcm_stat_object_t object,
    soc_mem_t table,
    int min_index,
    int max_index)
{
    uint32 profile_ptr, base_idx;
    uint32 entry_data_size=0;
    void *entry_data = NULL, *entry_full_data = NULL, *counter_entry_data = NULL;
    uint32 byte_flag;
    soc_mem_t counter_mem;
    int index=0, profile_index, pool_idx;
    int rv;
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    
    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc));

    rv = _bcm_esw_flowcnt_object_table_map(unit, object,
                                           &profile_index,
                                           &pool_idx);
    if (BCM_FAILURE(rv)){
        return rv;
    }
    _FLOWCNT_LOCK(fc);
    fc_profile = &fc->flowcnt_profile[profile_index];
    fc_pool = &fc->flowcnt_pool[pool_idx];

    entry_data_size = WORDS2BYTES(BYTES2WORDS(
                      SOC_MEM_INFO(unit, table).bytes));
    if (max_index > soc_mem_index_max(unit, table)) {
        max_index = soc_mem_index_max(unit, table);
    }
    _FLOWCNT_ALLOC(unit, entry_full_data, void, 
                   entry_data_size * (max_index - min_index + 1),
                   "object table", 1, rv);
    if (BCM_FAILURE(rv)){
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    rv = soc_mem_read_range(unit, table, MEM_BLOCK_ANY,
                            min_index, max_index, entry_full_data);
    if (BCM_FAILURE(rv)) {        
        _FLOWCNT_FREE(unit, entry_full_data, 1);
        _FLOWCNT_UNLOCK(fc);
        return rv;
    }

    for(index=0; index <= (max_index - min_index) ; index++) {
        entry_data = soc_mem_table_idx_to_pointer(unit, table,
                                                  void *, entry_full_data,
                                                  index);
        if (soc_mem_field_valid(unit, table, VALIDf)) {
            if (soc_mem_field32_get(unit, table, entry_data,
                VALIDf) == 0) {
                continue;
            }
        }
        rv = _bcm_esw_flowcnt_object_table_fields_values_get(
            unit, index, table, entry_data,
            &profile_ptr, &base_idx);
        if (BCM_FAILURE(rv)) {
            _FLOWCNT_FREE(unit, entry_full_data, 1);
            _FLOWCNT_UNLOCK(fc);
            return rv;
        }
        if ((profile_ptr == 0) && (base_idx == 0)) {
             continue;
        }
        fc_pgroup = &fc_profile->flowcnt_group[profile_ptr];
        if (fc_pgroup->available == 0) {
            fc_pgroup->available = 1;
        }
        fc_pool->flow_base_index_reference_count[base_idx]++;
        fc_pool->associate_profile_group[base_idx] = 
                    profile_ptr + _BCM_FLOWCNT_PGROUP_BIAS;
        fc_pool->object[base_idx] = object;
        if (fc_pool->flow_base_index_reference_count[base_idx] == 1) {
            fc_pgroup->reference_count++;
        }
        if (fc_pgroup-> total_counters == 0) {
            /* total_counters should be able recovered by flowcnt_scache_ptr*/
            rv = _bcm_esw_flowcnt_profile_table_retrieve(unit, profile_index, 
                                                         profile_ptr);
            if (BCM_FAILURE(rv)) {
                _FLOWCNT_UNLOCK(fc);
                _FLOWCNT_FREE(unit, entry_full_data, 1);
                return rv;
            }
        }
        shr_aidxres_list_reserve_block(
            fc_pool->flowcnt_aidxres_list_handle,
            base_idx,
            fc_pgroup->total_counters);
        fc_pool->pool_stat.used_entries += fc_pgroup->total_counters;
        fc_pool->pool_stat.attached_entries += fc_pgroup->total_counters;

        counter_mem = fc_pool->mem; 
        entry_data_size = WORDS2BYTES(BYTES2WORDS(
                          SOC_MEM_INFO(unit, counter_mem).bytes));
        _FLOWCNT_ALLOC(unit, counter_entry_data, void, entry_data_size,
                       "counter table entry", 0, rv);
        if (BCM_FAILURE(rv)){
            _FLOWCNT_UNLOCK(fc);
            _FLOWCNT_FREE(unit, entry_full_data, 1);
            return rv;
        }

        if (soc_mem_read(unit,
                         counter_mem,
                         MEM_BLOCK_ANY,
                         base_idx,
                         counter_entry_data) != BCM_E_NONE){
            _FLOWCNT_UNLOCK(fc);
            _FLOWCNT_FREE(unit, entry_full_data, 1);
            return BCM_E_INTERNAL;
        }

        soc_mem_field_get(unit,
                          counter_mem,
                          (uint32 *)counter_entry_data,
                          COUNT_BYTEf,
                          &byte_flag);
                
        if (byte_flag){
            fc_pool->flow_stat_type[base_idx]= bcmFlowcntStatTypeByte;
        } else {
            fc_pool->flow_stat_type[base_idx] = bcmFlowcntStatTypePacket;
        }
        LOG_DEBUG(BSL_LS_BCM_FLEXCTR,
                  (BSL_META_U(unit,
                              "Table:%s index=%d mode:%d pool_number:%d "
                              "base_idx:%d\n"),
                   SOC_MEM_UFNAME(unit, table),
                   index+min_index, profile_ptr, pool_idx, base_idx));
    }
    _FLOWCNT_UNLOCK(fc);
    _FLOWCNT_FREE(unit, entry_full_data, 1);
    _FLOWCNT_FREE(unit, counter_entry_data, 0);
    return BCM_E_NONE;
}
#define _BCM_FLOWCNT_WARMBOOT_READ_CHUNK 256
STATIC int
_bcm_esw_flowcnt_warmboot_object_retrive(int unit)
{
    int i, rv = BCM_E_NONE;
    uint32 index = 0;
    bcm_stat_object_t object;
    soc_mem_t table;
    for (i = 0; valid_stat_object_list[unit][i].object != -1; i++) {
        table =  valid_stat_object_list[unit][i].object_mem;
        object = valid_stat_object_list[unit][i].object;
        index = 0;

        if (soc_mem_index_max(unit, table) < 0) {
            continue;
        }

        do {
            rv = _bcm_esw_flowcnt_warmboot_object_table_retrieve(
                    unit, object, table, index, 
                    (index + _BCM_FLOWCNT_WARMBOOT_READ_CHUNK - 1));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            index += _BCM_FLOWCNT_WARMBOOT_READ_CHUNK;
        } while (index < soc_mem_index_count(unit,table));
    }
    return rv;
}
STATIC int 
_bcm_esw_flowcnt_warmboot_info_recover(int unit, uint8 *scache_ptr)
{
    _bcm_esw_flowcnt_control_t *fc = NULL;
    _bcm_esw_flowcnt_profile_t *fc_profile = NULL;
    _bcm_esw_flowcnt_profile_group_t *fc_pgroup = NULL;
    _bcm_esw_flowcnt_pool_t *fc_pool = NULL;
    _bcm_flowcnt_warmboot_info_t *flowcnt_wb_info;
    _bcm_flowcnt_warmboot_group_info_t *flowcnt_wb_group_info;
    int i, j, sel;
    uint32 stat_counter_id;
    bcm_stat_group_mode_t group_mode = bcmStatGroupModeSingle;
    bcm_stat_object_t object = bcmStatObjectIngPort;
    uint32 profile_group, pool_index, base_index;
    int profile_index, pool_idx;
    uint32 total_counters;
    int rv;
    int scache_size = 0;
    int scache_group_mode_num; /* the num of group_mode in scache */
    uint32 *ptr = (uint32 *)scache_ptr;
    uint32 default_attr_mode_id = 0;
    int p_group = 0;

    if (flowcnt_scache_allocated_size[unit] == 0) {
         LOG_ERROR(BSL_LS_BCM_FLEXCTR,
                   (BSL_META_U(unit,
                               "allocated size error\n")));
         return BCM_E_CONFIG;
    }
    
    scache_size = FLOWCNT_SCACHE_SIZE_1;
    flowcnt_wb_info = (_bcm_flowcnt_warmboot_info_t *)
                       (scache_ptr + scache_size);
    scache_group_mode_num = FLOWCNT_SCACHE_SIZE_2_RECOVER(unit) / sizeof(uint32);
    scache_size += FLOWCNT_SCACHE_SIZE_2_RECOVER(unit);
    flowcnt_wb_group_info = (_bcm_flowcnt_warmboot_group_info_t *)
                             (scache_ptr + scache_size);

    BCM_IF_ERROR_RETURN(_bcm_esw_flowcnt_control_get(unit, &fc)); 
    _FLOWCNT_LOCK(fc); 

    if (_bcm_esw_stat_sync_version_above_equal(unit, BCM_WB_VERSION_1_3)) {
        LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                    (BSL_META_U(unit,
                                "NEW Version\n")));
        if (_bcm_esw_stat_sync_version_above_equal(unit,
                                                   BCM_WB_VERSION_1_10)) {
            /*
             * The default_attr_mode_id is extented to two dimensional array when
             * WB version >= BCM_WB_VERSION_1_10.
             */
            for (i = 0; i < fc->num_profiles; i++) {
                for (j = 0; j < bcmStatGroupModeAttrMaxValue; j++) {
                    if (j >= scache_group_mode_num) {
                        break;
                    }
                    sal_memcpy(&fc->default_attr_mode_id[i][j],
                               &flowcnt_wb_info->default_attr_mode_id[i][j],
                               sizeof(uint32));
                }
            }
        } else {
            /*
             * The default_attr_mode_id is one dimensional array when
             * WB version between BCM_WB_VERSION_1_3 and BCM_WB_VERSION_1_9.
             */
            for (i = 0; i < bcmStatGroupModeAttrMaxValue; i++) {
                if (i >= scache_group_mode_num) {
                    break;
                }
                default_attr_mode_id =
                    flowcnt_wb_info->default_attr_mode_id[0][i];
                default_attr_mode_id &= ~(_BCM_FLOWCNT_MODE_ID_VALID);
                _bcm_esw_flowcnt_custom_mode_id_get(default_attr_mode_id,
                                                    &profile_index,
                                                    &p_group);
                sal_memcpy(&fc->default_attr_mode_id[profile_index][i],
                           &flowcnt_wb_info->default_attr_mode_id[0][i],
                           sizeof(uint32));
            }
        }

        for (i = 0; i < fc->num_profiles; i++) {
            fc_profile = &fc->flowcnt_profile[i];
            for (j = 0; j < fc_profile->num_groups; j++) {
                fc_pgroup = &fc_profile->flowcnt_group[j];
                fc_pgroup->group_mode = flowcnt_wb_group_info->group_mode;
                fc_pgroup->flags = flowcnt_wb_group_info->flags;
                fc_pgroup->total_counters = flowcnt_wb_group_info->total_counters;
                fc_pgroup->num_selectors = flowcnt_wb_group_info->num_selectors;
                if (fc_pgroup->num_selectors) {
                    fc_pgroup->available = 1;
                    rv = soc_profile_mem_reference(unit,
                                                   &fc_profile->profile_mem,
                                                   j*fc_profile->num_entries,
                                                   1);
                    if (BCM_FAILURE(rv)){
                        _FLOWCNT_UNLOCK(fc);
                        return rv;
                    }
                    _FLOWCNT_ALLOC(unit, fc_pgroup->attr_selectors,
                                   _bcm_flowcnt_group_mode_attr_selector_t,
                                   fc_pgroup->num_selectors *
                                   sizeof(_bcm_flowcnt_group_mode_attr_selector_t),
                                   "flowcnt pkt attr per profile group", 0, rv);
                    if (BCM_FAILURE(rv)){
                        _FLOWCNT_UNLOCK(fc);
                        return rv;
                    }
                    for (sel = 0; sel < fc_pgroup->num_selectors; sel ++) {
                        sal_memcpy(&fc_pgroup->attr_selectors[sel],
                                   &flowcnt_wb_group_info->attr_selectors[sel],
                                   sizeof(_bcm_flowcnt_group_mode_attr_selector_t));
                    }
                }
                flowcnt_wb_group_info++;
            }
        }
    } else {
        LOG_VERBOSE(BSL_LS_BCM_FLEXCTR,
                    (BSL_META_U(unit,
                                "OLD Version so skipping NEW FLEX Layout..\n")));
    }
    for (i = 0; i < MAX_FLOWCNT_SCACHE_SIZE; i++) {
        if (ptr[i] != 0) {
            stat_counter_id = ptr[i];
            _bcm_esw_flowcnt_counter_id_get(stat_counter_id, &group_mode,
                                            &object,&profile_group, &pool_index,
                                            &base_index);
            if (_bcm_esw_flowcnt_object_table_map(
                    unit, object, &profile_index, &pool_idx) != BCM_E_NONE){
                continue;
            }
            fc_profile = &fc->flowcnt_profile[profile_index];
            fc_pgroup = &fc_profile->flowcnt_group[profile_group];
            fc_pool = &fc->flowcnt_pool[pool_idx];
            total_counters = fc_pgroup->total_counters;
            if (total_counters == 0) {
                continue;
            }
            shr_aidxres_list_reserve_block(
                fc_pool->flowcnt_aidxres_list_handle,
                base_index,
                total_counters);
            fc_pool->pool_stat.used_entries += total_counters;
            fc_pgroup->reference_count++;
        }
    }
    _FLOWCNT_UNLOCK(fc);
    return BCM_E_NONE;
}
#endif

