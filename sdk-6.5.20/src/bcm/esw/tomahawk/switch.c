/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module : Switch
 *
 * Purpose:
 * Aggregation Monitor Management.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>
#include <soc/iproc.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/time.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw_dispatch.h>

/* Definitions */

#define AGM_CHECK(unit, agm_id) \
	if ((agm_id) < AGM_CNTL(unit).agm_id_min || (agm_id) > AGM_CNTL(unit).agm_id_max) \
        { return BCM_E_PARAM; }
#define AGM_LOCK(unit)        soc_mem_lock(unit, AGM_MONITOR_TABLEm)
#define AGM_UNLOCK(unit)      soc_mem_unlock(unit, AGM_MONITOR_TABLEm)

/* Data structure */
typedef struct agm_cntl_s {
    int             agm_id_min;
    int             agm_id_max;
    int             agm_period_max;
    int             agm_timer_id;
    int             agm_pool_conf[_AGM_POOL_ID_CNT];
    agm_monitor_t   *agm_info;
} agm_cntl_t;

/*
 * One agm control entry for each SOC device containing AGM book keeping
 * info for that device.
 */
static agm_cntl_t bcm_agm_control[BCM_MAX_NUM_UNITS];

#define AGM_CNTL(unit)	        bcm_agm_control[unit]
#define AGM_INFO(unit, aid)     bcm_agm_control[unit].agm_info[(aid)]

/*
 * Cause a routine to return BCM_E_INIT if agm subsystem is not
 * initialized.
 */
#define AGM_INIT(unit)	                    \
	if ( (AGM_CNTL(unit).agm_id_max <= 0) && \
	     (AGM_CNTL(unit).agm_period_max <= 0) ) { return BCM_E_INIT; }
/*
 * 0: ECMP1, ECMP2
 * 1: ECMP1, TRUNK
 * 2: ECMP1, HG-TRUNK
 * 3: TRUNK, HG-TRUNK
 */
#define _POOL_EMPTY(unit, pid) \
    (AGM_CNTL(unit).agm_pool_conf[(pid)] == _AGM_POOL_ID_INVALID)
#define _POOL_CHECK(unit, pid, type) \
    (AGM_CNTL(unit).agm_pool_conf[(pid)] == (type))

/* Functions */

int
bcm_th_switch_agm_deinit(int unit)
{
    if (AGM_CNTL(unit).agm_info != NULL) {
        sal_free(AGM_CNTL(unit).agm_info);
    }
    sal_memset(&AGM_CNTL(unit), 0, sizeof(AGM_CNTL(unit)));

    return BCM_E_NONE;
}

int
bcm_th_switch_agm_id_validate(int unit, bcm_switch_agm_id_t agm_id) {
    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);
    return BCM_E_NONE;
}

int
_bcm_th_switch_agm_update_counter_id(
    int unit, bcm_switch_agm_id_t agm_id, int counter_id)
{
    int rv = BCM_E_NONE;

    AGM_CHECK(unit, agm_id);

    if (AGM_INFO(unit, agm_id).in_use) {
        AGM_INFO(unit, agm_id).counter_id = counter_id;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

int
_bcm_th_switch_agm_info(int unit, bcm_switch_agm_id_t agm_id,
                       agm_monitor_t *agm_mnt)
{
    int rv = BCM_E_NONE;

    if (AGM_INFO(unit, agm_id).in_use) {
        sal_memcpy(agm_mnt, &AGM_INFO(unit, agm_id), sizeof(*agm_mnt));
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

int
bcm_th_switch_agm_init(int unit)
{
    int i, agm_alloc_sz;

    soc_mem_t agm_tbl = AGM_MONITOR_TABLEm;

    AGM_CNTL(unit).agm_id_min = soc_mem_index_min(unit, agm_tbl);
    AGM_CNTL(unit).agm_id_max = soc_mem_index_max(unit, agm_tbl);
    AGM_CNTL(unit).agm_period_max = (1 << soc_mem_field_length(unit, agm_tbl,
                                    PERIOD_MAXf)) - 1;
    AGM_CNTL(unit).agm_timer_id = _AGM_TIME_ID_INVALID;
    for (i = 0; i < _AGM_POOL_ID_CNT; i++) {
        AGM_CNTL(unit).agm_pool_conf[i] = _AGM_POOL_ID_INVALID;
    }

    if (AGM_CNTL(unit).agm_info != NULL) {
        sal_free(AGM_CNTL(unit).agm_info);
        AGM_CNTL(unit).agm_info = NULL;
    }

    agm_alloc_sz = (AGM_CNTL(unit).agm_id_max - AGM_CNTL(unit).agm_id_min + 1) *
                   sizeof(agm_monitor_t);
    AGM_CNTL(unit).agm_info = sal_alloc(agm_alloc_sz, "agm_cntl.info");
    if (AGM_CNTL(unit).agm_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(AGM_CNTL(unit).agm_info, 0, agm_alloc_sz);

    for (i = 0; i <= (AGM_CNTL(unit).agm_id_max - AGM_CNTL(unit).agm_id_min);
         i++) {
        AGM_INFO(unit, i).attr.agm_id = _AGM_ID_INVALID;
        AGM_INFO(unit, i).group_id = _AGM_GROUP_ID_INVALID;
    }

    return BCM_E_NONE;
}

#if defined (BCM_WARM_BOOT_SUPPORT)
int
bcm_th_switch_tflow_wb_scache_size_get(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += sizeof(_bcm_switch_tflow_mode_t);
    *req_scache_size += alloc_size;

    return BCM_E_NONE;
}

int
bcm_th_switch_tflow_wb_sync(int unit, uint8 **scache_ptr)
{
    _bcm_switch_tflow_mode_t *tflow_scache_p;

    tflow_scache_p = (_bcm_switch_tflow_mode_t *)(*scache_ptr);

    sal_memcpy(tflow_scache_p, &(_bcm_switch_tflow_mode_info[unit]), sizeof(_bcm_switch_tflow_mode_t));
    tflow_scache_p++;

    *scache_ptr = (uint8 *)tflow_scache_p;

    return BCM_E_NONE;
}

int
bcm_th_switch_tflow_wb_reinit(int unit, uint8 **scache_ptr)
{
    _bcm_switch_tflow_mode_t *tflow_scache_p;

    tflow_scache_p = (_bcm_switch_tflow_mode_t *)(*scache_ptr);

    sal_memcpy(&(_bcm_switch_tflow_mode_info[unit]), tflow_scache_p, sizeof(_bcm_switch_tflow_mode_t));
    tflow_scache_p++;

    *scache_ptr = (uint8 *)tflow_scache_p;

    return BCM_E_NONE;
}


int
bcm_th_switch_agm_wb_scache_size_get(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += sizeof(agm_cntl_t);
    alloc_size += ((AGM_CNTL(unit).agm_id_max - AGM_CNTL(unit).agm_id_min + 1) *
                   sizeof(agm_monitor_t));
    
    *req_scache_size += alloc_size;

    return BCM_E_NONE;
}

int
bcm_th_switch_agm_wb_sync(int unit, uint8 **scache_ptr)
{
    agm_cntl_t *agm_cntl_scache_p;
    agm_monitor_t * agm_info_scache_p;
    int agm_info_num;

    agm_cntl_scache_p = (agm_cntl_t *)(*scache_ptr);
    
    sal_memcpy(agm_cntl_scache_p, &AGM_CNTL(unit), sizeof(agm_cntl_t));
    agm_cntl_scache_p ++;

    agm_info_scache_p = (agm_monitor_t *)agm_cntl_scache_p;
    agm_info_num = AGM_CNTL(unit).agm_id_max - AGM_CNTL(unit).agm_id_min + 1;
    sal_memcpy(agm_info_scache_p, AGM_CNTL(unit).agm_info, 
        agm_info_num * sizeof(agm_monitor_t));
    agm_info_scache_p += agm_info_num;

    *scache_ptr = (uint8 *)agm_info_scache_p;
    
    return BCM_E_NONE;
}

int
bcm_th_switch_agm_wb_reinit(int unit, uint8 **scache_ptr)
{
    agm_cntl_t *agm_cntl_scache_p;
    agm_monitor_t * agm_info_scache_p;
    int agm_info_num;

    agm_cntl_scache_p = (agm_cntl_t *)(*scache_ptr);

    sal_memcpy(&AGM_CNTL(unit), agm_cntl_scache_p, sizeof(agm_cntl_t));
    agm_cntl_scache_p ++;

    agm_info_scache_p = (agm_monitor_t *)agm_cntl_scache_p;
    agm_info_num = AGM_CNTL(unit).agm_id_max - AGM_CNTL(unit).agm_id_min + 1;
    AGM_CNTL(unit).agm_info = sal_alloc(agm_info_num * sizeof(agm_monitor_t), 
        "agm_cntl.info");
    if (AGM_CNTL(unit).agm_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memcpy(AGM_CNTL(unit).agm_info, agm_info_scache_p, 
        agm_info_num * sizeof(agm_monitor_t));
    agm_info_scache_p += agm_info_num;

    *scache_ptr = (uint8 *)agm_info_scache_p;
    
    return BCM_E_NONE;
}
#endif 

int
bcm_th_switch_agm_dump(int unit)
{
    /* For trouble-shooting */
    AGM_INIT(unit);

    return BCM_E_NONE;
}

/*
 * 0: ECMP, ECMP
 * 1: ECMP, TRUNK
 * 2: ECMP, HG-TRUNK
 * 3: TRUNK, HG-TRUNK
 */
STATIC int
_th_agm_pool_alloc(int unit, bcm_switch_agm_type_t agm_type, int *pool_id)
{
    int i;

    for (i = 0; i < _AGM_POOL_ID_CNT; i++) {
        if (_POOL_EMPTY(unit, i) || _POOL_CHECK(unit, i, agm_type)) {
            *pool_id = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_FULL;

}

STATIC int
_th_agm_id_alloc(int unit, int *agm_id)
{
    int i;

    for (i = AGM_CNTL(unit).agm_id_min + 1; i <= AGM_CNTL(unit).agm_id_max; i++) {
        if (AGM_INFO(unit, i).attr.agm_id != _AGM_ID_INVALID) {
            continue;
        }
        *agm_id = i;
        return BCM_E_NONE;
    }

    return BCM_E_FULL;
}

STATIC int
_th_agm_itvl2scale(int unit, bcm_switch_agm_period_interval_t itvl)
{
    int scale = 0;

    switch (itvl) {
    case bcmSwitchAgmInterval1Second:
        scale = 5;
        break;
    case bcmSwitchAgmInterval100MiliSecond:
        scale = 4;
        break;
    case bcmSwtichAgmInterval10MiliSecond:
        scale = 3;
        break;
    case bcmSwitchAgmInterval1MiliSecond:
        scale = 2;
        break;
    case bcmSwitchAgmInterval100MacroSecond:
        scale = 1;
        break;
    default:
        break;
    }

    return scale;
}

/* Retrieve table info for AGM instance */
int
bcm_th_agm_stat_get_table_info(int unit, bcm_switch_agm_id_t agm_id,
                                uint32 *num_of_tables, void *tbl_info)
{
    int index = agm_id;
    bcm_stat_flex_table_info_t *table_info = NULL;

    (*num_of_tables) = 0;
    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    table_info = (bcm_stat_flex_table_info_t *) tbl_info;
    table_info[*num_of_tables].table        = AGM_MONITOR_TABLEm;
    table_info[*num_of_tables].index        = index;
    table_info[*num_of_tables].direction    = bcmStatFlexDirectionIngress;
    (*num_of_tables)++;

    return BCM_E_NONE;
}

/* Get counter id for accounnting table AGM */
int
bcm_th_agm_stat_id_get(int unit, bcm_switch_agm_id_t agm_id,
                    int agm_pid, int total_cntr_idxs, uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;

    bcm_stat_group_mode_id_config_t stat_config;
    bcm_stat_group_mode_attr_selector_t attr_sel[1];
    uint32 mode_id;
    uint32 num_entries;
    uint32 num_sel;

    if (!soc_feature(unit,soc_feature_advanced_flex_counter)) {
        return BCM_E_UNAVAIL;
    }

    bcm_stat_group_mode_id_config_t_init(&stat_config);
    bcm_stat_group_mode_attr_selector_t_init(attr_sel);

    stat_config.flags = BCM_STAT_GROUP_MODE_INGRESS;
    stat_config.total_counters = total_cntr_idxs;
    stat_config.hint.type = bcmIntStatGroupAllocHintIngressAgmGroup;
    stat_config.hint.value = 1;

    /* Create custom mode id with config */
    attr_sel[0].attr = 0;
    attr_sel[0].attr_value = 0;
    attr_sel[0].counter_offset = 0;
    num_sel = 1;
    rv = bcm_esw_stat_group_mode_id_config_create(unit, 0, &stat_config,
            num_sel, attr_sel, &mode_id);
    if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
        return rv;
    }

    /* Create counter id */
    if (agm_pid) {
        rv = bcm_esw_stat_custom_group_create(unit, mode_id,
                (bcm_stat_object_t)bcmIntStatObjectIngAgmSecondLookup,
                stat_counter_id, &num_entries);
    } else {
        rv = bcm_esw_stat_custom_group_create(unit, mode_id,
                (bcm_stat_object_t)bcmIntStatObjectIngAgm,
                stat_counter_id, &num_entries);
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Make sure there is space there */
    if (total_cntr_idxs > num_entries) {
        BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(unit, *stat_counter_id));
        return BCM_E_RESOURCE;
    }

    return rv;
}

/* Destroy AGM counter id */
int
bcm_th_agm_stat_id_clear(int unit, uint32 stat_counter_id)
{
    bcm_stat_group_mode_t gmode;
    bcm_stat_object_t stat_object;
    uint32 mode, pool, base_idx;
    int rv;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) && !stat_counter_id) {
        return BCM_E_NONE;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    _bcm_esw_stat_get_counter_id_info(unit, stat_counter_id, &gmode,
                                      &stat_object, &mode, &pool, &base_idx);

    BCM_IF_ERROR_RETURN(bcm_esw_stat_group_destroy(unit, stat_counter_id));

    /* This function could return BUSY if the reference count is non-zero */
    rv = _bcm_esw_stat_group_mode_id_destroy(unit, mode);
    if (rv == BCM_E_BUSY)
        return BCM_E_NONE;
    return rv;
}

/* Detach Flex counter from AGM monitor */
int
bcm_th_agm_stat_detach(int unit,
    bcm_switch_agm_id_t agm_id, int agm_pid, uint32 ctr_id)
{
    soc_mem_t                  table;
    bcm_stat_flex_direction_t  direction;
    bcm_stat_object_t          object;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) && !ctr_id) {
        return BCM_E_NONE;
    }
#else
    COMPILER_REFERENCE(ctr_id);
#endif /* BCM_TOMAHAWK3_SUPPORT */

    direction   = bcmStatFlexDirectionIngress;
    if (agm_pid) {
        object      = bcmIntStatObjectIngAgmSecondLookup;
    } else {
        object      = bcmIntStatObjectIngAgm;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(unit, object, 1,
                            &actual_num_tables, &table, &direction));

    BCM_IF_ERROR_RETURN(bcm_th_agm_stat_get_table_info(unit, agm_id,
                        &num_of_tables, &table_info[0]));
    for (count = 0; count < num_of_tables; count++) {
        if ((table_info[count].direction == direction) &&
            (table_info[count].table == table)) {
            if (direction == bcmStatFlexDirectionIngress) {
                return _bcm_esw_stat_flex_detach_ingress_table_counters1(
                            unit, table_info[count].table,
                            table_info[count].index, object);
            } else {
                return _bcm_esw_stat_flex_detach_egress_table_counters(
                            unit, 0, table_info[count].table,
                            table_info[count].index);
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/* Attach a flex counter to AGM monitor */
int
bcm_th_agm_stat_attach(int unit, bcm_switch_agm_id_t agm_id,
                    int agm_pid, uint32 counter_id)
{
    soc_mem_t                  table;
    bcm_stat_flex_direction_t  direction;
    uint32                     pool_number = 0;
    uint32                     base_index = 0;
    bcm_stat_flex_mode_t       offset_mode = 0;
    bcm_stat_object_t          object;
    bcm_stat_group_mode_t      group_mode;
    uint32                     count = 0;
    uint32                     actual_num_tables = 0;
    uint32                     num_of_tables = 0;
    bcm_stat_flex_table_info_t table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    direction   = bcmStatFlexDirectionIngress;
    if (agm_pid) {
        object      = bcmIntStatObjectIngAgmSecondLookup;
    } else {
        object      = bcmIntStatObjectIngAgm;
    }
    group_mode  = bcmStatGroupModeSingle;

    _bcm_esw_stat_get_counter_id_info(unit, counter_id, &group_mode, &object,
                                      &offset_mode, &pool_number, &base_index);
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_object(unit, object, &direction));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_validate_group(unit, group_mode));
    BCM_IF_ERROR_RETURN(_bcm_esw_stat_flex_get_table_info(unit, object, 1,
                            &actual_num_tables, &table, &direction));

    BCM_IF_ERROR_RETURN(bcm_th_agm_stat_get_table_info(unit, agm_id,
                        &num_of_tables, &table_info[0]));
    for (count = 0; count < num_of_tables ; count++) {
        if ((table_info[count].direction == direction) &&
            (table_info[count].table == table)) {
            if (direction == bcmStatFlexDirectionIngress) {
                return _bcm_esw_stat_flex_attach_ingress_table_counters2(
                            unit, table_info[count].table,
                            table_info[count].index, offset_mode, base_index,
                            pool_number, object, NULL);
            } else {
                return _bcm_esw_stat_flex_attach_egress_table_counters(
                            unit, table_info[count].table,
                            table_info[count].index, 0, offset_mode, base_index,
                            pool_number);
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

/* Fill trunk member to AGM stat array */
STATIC int
_th_agm_trunk_member_fill(int unit, bcm_switch_agm_id_t agm_id,
                          bcm_trunk_t tid, int nstat,
                          bcm_switch_agm_stat_t *stat_arr)
{
    int i, rv = BCM_E_NONE;
    bcm_trunk_info_t tinfo;
    int mbm_cnt, mbm_max;
    bcm_trunk_member_t *mbm_arr;

    mbm_max = AGM_INFO(unit, agm_id).attr.num_members;
    mbm_arr = sal_alloc(mbm_max * sizeof(*mbm_arr), "mbm_arr");
    if (mbm_arr == NULL) {
        return BCM_E_MEMORY;
    }

    rv = bcm_esw_trunk_get(unit, tid, &tinfo, mbm_max, mbm_arr, &mbm_cnt);
    if (BCM_FAILURE(rv)) {
        sal_free(mbm_arr);
        return rv;
    }

    if (mbm_cnt == 0) {
        sal_free(mbm_arr);
        return BCM_E_FAIL;
    }

    /* Could cross multi monitor period */
    for (i = 0; i < nstat; i++) {
        stat_arr[i].intf_id = BCM_IF_INVALID;
        stat_arr[i].gport_id = mbm_arr[i % mbm_cnt].gport;
    }

    sal_free(mbm_arr);

    return BCM_E_NONE;

}

#if defined(INCLUDE_L3)
/* Fill l3 ecmp member to AGM stat array */
STATIC int
_th_agm_l3_ecmp_member_fill(int unit, bcm_switch_agm_id_t agm_id,
                            bcm_if_t l3_ecmp_id, int nstat,
                            bcm_switch_agm_stat_t *stat_arr)
{
    int i, rv = BCM_E_NONE;
    int mbm_cnt = 0, mbm_max;
    bcm_if_t *mbm_arr;

    mbm_max = AGM_INFO(unit, agm_id).attr.num_members;
    mbm_arr = sal_alloc(mbm_max * sizeof(*mbm_arr), "mbm_arr");
    if (mbm_arr == NULL) {
        return BCM_E_MEMORY;
    }

    rv = bcm_esw_l3_egress_multipath_get(unit, l3_ecmp_id, mbm_max,
                                         mbm_arr, &mbm_cnt);
    if (BCM_FAILURE(rv)) {
        sal_free(mbm_arr);
        return rv;
    }

    if (mbm_cnt == 0) {
        sal_free(mbm_arr);
        return BCM_E_FAIL;
    }

    /* Could cross multi monitor period */
    for (i = 0; i < nstat; i++) {
        stat_arr[i].gport_id = BCM_GPORT_INVALID;
        stat_arr[i].intf_id  = mbm_arr[i % mbm_cnt];
    }

    sal_free(mbm_arr);

    return BCM_E_NONE;

}
#endif

/*
 * Function:
 *      bcm_th_switch_agm_create
 * Purpose:
 *      Creates an aggregation monitoring entry which collects bandwidth and
 *      packet distribution information among members of a trunk group or
 *      an ecmp group.
 */
int
bcm_th_switch_agm_create(int unit,
                         uint32 options,
                         bcm_switch_agm_info_t *agm_info)
{
    int rv = BCM_E_NONE;
    int agm_pid, agm_id;
    agm_monitor_table_entry_t entry;
    uint32 counter_id = 0;
    agm_monitor_t *agm_mng;
    int scale;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AGM_INIT(unit);

    if (agm_info == NULL) {
        return BCM_E_PARAM;
    }

    if (agm_info->period_interval < bcmSwitchAgmInterval1Second ||
        agm_info->period_interval >= bcmSwitchAgmIntervalCount) {
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                  "Invalid period interval %d\n"), agm_info->period_interval));
        return BCM_E_PARAM;
    }

    if (agm_info->period_num < 0 ||
        agm_info->period_num > AGM_CNTL(unit).agm_period_max) {
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                  "Invalid period number %d\n"), agm_info->period_num));
        return BCM_E_PARAM;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        if(bcmSwitchAgmTypeFabricTrunk == agm_info->agm_type) {
            return BCM_E_PARAM;
        }
        if (agm_info->num_members <= 0) {
            return BCM_E_PARAM;
        }
    }
#endif

    AGM_LOCK(unit);

    /* Check available agm pool */
    rv = _th_agm_pool_alloc(unit, agm_info->agm_type, &agm_pid);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        return rv;
    }

    /* Alloc monitor id */
    rv = _th_agm_id_alloc(unit, &agm_id);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        return rv;
    }

    LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "Allocated AGM pool id %d, monitor id %d\n"), agm_pid, agm_id));

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* For TH3, skip creating Flex Stat counters for ECMP
     * AGMs as it will be taken care at ECMP attach phase.
     */
    if ((!SOC_IS_TOMAHAWK3(unit)) ||
        (bcmSwitchAgmTypeL3Ecmp != agm_info->agm_type &&
         bcmSwitchAgmTypeL3EcmpOverlay != agm_info->agm_type))
#endif /* BCM_TOMAHAWK3_SUPPORT */
    {

        rv = bcm_th_agm_stat_id_get(unit,
                                 agm_id,
                                 agm_pid,
                                 agm_info->num_members * (agm_info->period_num + 1),
                                 &counter_id);
        if (BCM_FAILURE(rv)) {
            AGM_UNLOCK(unit);
            return rv;
        }

        LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "Allocated counter id %d for AGM accouting table\n"), counter_id));

        rv = bcm_th_agm_stat_attach(unit, agm_id, agm_pid, counter_id);
        if (BCM_FAILURE(rv)) {
            bcm_th_agm_stat_id_clear(unit, counter_id);
            AGM_UNLOCK(unit);
            return rv;
        }

        LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "Attached counter id %d to AGM %d\n"), counter_id, agm_id));
    }

    rv = READ_AGM_MONITOR_TABLEm(unit, MEM_BLOCK_ANY, agm_id, &entry);
    if (BCM_FAILURE(rv)) {
        (void)bcm_th_agm_stat_detach(unit, agm_id, agm_pid, counter_id);
        (void)bcm_th_agm_stat_id_clear(unit, counter_id);
        AGM_UNLOCK(unit);
        return rv;
    }

    /* For unlimited period, we need to set SCALE to 0 */
    if (agm_info->period_num == 0) {
        scale = 0;
    } else {
        scale = _th_agm_itvl2scale(unit, agm_info->period_interval);
    }
    soc_mem_field32_set(unit, AGM_MONITOR_TABLEm, &entry, SCALEf, scale);
    soc_mem_field32_set(unit, AGM_MONITOR_TABLEm, &entry, PERIOD_MAXf,
                        agm_info->period_num);
    rv = WRITE_AGM_MONITOR_TABLEm(unit, MEM_BLOCK_ANY, agm_id, &entry);
    if (BCM_FAILURE(rv)) {
        (void)bcm_th_agm_stat_detach(unit, agm_id, agm_pid, counter_id);
        (void)bcm_th_agm_stat_id_clear(unit, counter_id);
        AGM_UNLOCK(unit);
        return rv;
    }

    /* Update software bookkeeping information */
    AGM_CNTL(unit).agm_pool_conf[agm_pid] = agm_info->agm_type;
    agm_mng = &AGM_INFO(unit, agm_id);
    agm_mng->in_use         = TRUE;
    agm_mng->agm_pool_id    = agm_pid;
    agm_mng->counter_id     = counter_id;

    sal_memcpy(&agm_mng->attr, agm_info, sizeof(*agm_info));
    agm_mng->attr.agm_id    = agm_id;
    agm_info->agm_id        = agm_id;

    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_enable_set
 * Purpose:
 *      Enable or disable the aggregation group monitoring.
 */
int
bcm_th_switch_agm_enable_set(int unit,
                             bcm_switch_agm_id_t agm_id,
                             int enable)
{
    int i, rv = BCM_E_NONE;
    agm_monitor_table_entry_t entry;
    uint32 start_ts[2] = {0};
    bcm_time_interface_t time_intf;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (!AGM_INFO(unit, agm_id).in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    /* Reentry check */
    if ((enable && AGM_INFO(unit, agm_id).running) ||
        (!enable && !AGM_INFO(unit, agm_id).running)) {
        AGM_UNLOCK(unit);
        return BCM_E_NONE;
    }

    /* Enable timer for the first time */
    if (enable &&
        AGM_INFO(unit, agm_id).attr.period_num > 0) {
        time_intf.id = _AGM_TIME_ID_DFLT;

        if (AGM_CNTL(unit).agm_timer_id == _AGM_TIME_ID_INVALID) {
            time_intf.flags = BCM_TIME_ENABLE | BCM_TIME_WITH_ID;
            sal_memset(&time_intf.drift, 0, sizeof(time_intf.drift));
            sal_memset(&time_intf.offset, 0, sizeof(time_intf.offset));
            sal_memset(&time_intf.accuracy, 0, sizeof(time_intf.accuracy));
            time_intf.heartbeat_hz      = 4000;
            time_intf.clk_resolution    = 0;
            time_intf.bitclock_hz       = 10000000;

            rv = bcm_esw_time_interface_add(unit, &time_intf);
            if (BCM_FAILURE(rv) && rv != BCM_E_EXISTS) {
                AGM_UNLOCK(unit);
                return rv;
            }

            AGM_CNTL(unit).agm_timer_id = _AGM_TIME_ID_DFLT;

            LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "Created a time interface id %d for AGM %d.\n"),
                AGM_CNTL(unit).agm_timer_id, agm_id));\
        }

#if defined(BCM_TOMAHAWK3_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIREBOLT6(unit)) {
            READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &start_ts[1]);
            READ_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit, &start_ts[0]);
            READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &start_ts[1]);
            start_ts[0] =
                (start_ts[0] >> 4 & 0x0FFFFFFF) | ((start_ts[1] & 0xF) << 28);
            start_ts[1] >>= 4;
        } else
#endif
        {
            bcm_time_capture_t time1;
            bcm_time_capture_t_init(&time1);
            (void) bcm_esw_time_capture_get(unit, time_intf.id, &time1);

            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &start_ts[1]);
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &start_ts[0]);
        }
    } else if (!enable || AGM_INFO(unit, agm_id).attr.period_num <= 0) {
        start_ts[0] = 0;
        start_ts[1] = 0;
    }

    LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "Start time stamp 0x%x_0x%x for AGM %d.\n"), start_ts[1],
                start_ts[0], agm_id));

    /* Update to AGM monitor table */
    rv = READ_AGM_MONITOR_TABLEm(unit, MEM_BLOCK_ANY, agm_id, &entry);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        return rv;
    }

    soc_mem_field_set(unit, AGM_MONITOR_TABLEm, (uint32 *)&entry,
                      START_TIMESTAMPf, start_ts);
    soc_mem_field32_set(unit, AGM_MONITOR_TABLEm, &entry, ENABLEf, enable);
    rv = WRITE_AGM_MONITOR_TABLEm(unit, MEM_BLOCK_ANY, agm_id, &entry);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        return rv;
    }

    /* Update running state */
    AGM_INFO(unit, agm_id).running = enable ? TRUE : FALSE;

    /* Destroy time interface if all monitors disabled. */
    if (!enable && AGM_CNTL(unit).agm_timer_id != _AGM_TIME_ID_INVALID) {
        for (i = AGM_CNTL(unit).agm_id_min;
             i <= AGM_CNTL(unit).agm_id_max; i++) {
            if (AGM_INFO(unit, i).running) {
                break;
            }
        }

        /* All AGM instances are not running, remove time interface. */
        if (i > AGM_CNTL(unit).agm_id_max) {
            rv = bcm_esw_time_interface_delete(unit, AGM_CNTL(unit).agm_timer_id);
            if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
                AGM_UNLOCK(unit);
                return rv;
            }

            LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "All AGM monitor are stopped, delete time interface.\n")));

            AGM_CNTL(unit).agm_timer_id = _AGM_TIME_ID_INVALID;
        }
    }

    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_stat_get
 * Purpose:
 *      Retrieve bandwidth and packet distribution information that were
 *      collected via aggregation group monitoring
 */
int
bcm_th_switch_agm_stat_get(int unit,
                           bcm_switch_agm_id_t agm_id,
                           int nstat,
                           bcm_switch_agm_stat_t *stat_arr)
{
    int     i, rv = BCM_E_NONE;
    int     group_size;
    uint32  *counter_indexes = NULL;
    bcm_stat_value_t *counter_values = NULL;
    bcm_switch_agm_type_t agm_type;

    uint32  num_of_tables = 0;
    int     sync_mode = TRUE;
    int     byte_flag;
    bcm_stat_flex_direction_t   direction = bcmStatFlexDirectionIngress;
    bcm_stat_flex_table_info_t  table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (stat_arr == NULL) {
        return BCM_E_PARAM;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);

    if (!AGM_INFO(unit, agm_id).in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    counter_indexes = sal_alloc(sizeof(uint32) * nstat, "cntidx");
    if (counter_indexes == NULL) {
        AGM_UNLOCK(unit);
        return BCM_E_MEMORY;
    }
    counter_values = sal_alloc(sizeof(bcm_stat_value_t) * nstat, "cntval");
    if (counter_values == NULL) {
        sal_free(counter_indexes);
        AGM_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    group_size = AGM_INFO(unit, agm_id).attr.num_members;
    for (i = 0; i < nstat; i++) {
        counter_indexes[i] = i;
        stat_arr[i].period = i / group_size;
    }
    sal_memset(counter_values, 0, sizeof(bcm_stat_value_t) * nstat);

    LOG_VERBOSE(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
        "AGM %d stat get: group member count %d, period %d.\n"), agm_id,
        group_size, (nstat / group_size) + 1 ));

    rv = bcm_th_agm_stat_get_table_info(
            unit, agm_id, &num_of_tables, &table_info[0]);
    if (BCM_FAILURE(rv)) {
        goto _free_exit;
    }

    if (table_info[0].direction == direction) {
        /* Collect packet counters */
        byte_flag = 0;
        for (i = 0; i < nstat; i++) {
            rv = _bcm_esw_stat_counter_get(unit, sync_mode, table_info[0].index,
                    table_info[0].table, 0, byte_flag, counter_indexes[i],
                    &counter_values[i]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "AGM %d collected packet counter %d failed, rv = %d.\n"),
                    agm_id, i, rv));
                goto _free_exit;
            }
            stat_arr[i].packets = counter_values[i].packets64;
        }

        /* Collect byte counters */
        byte_flag = 1;
        for (i = 0; i < nstat; i++) {
            rv = _bcm_esw_stat_counter_get(unit, sync_mode, table_info[0].index,
                    table_info[0].table, 0, byte_flag, counter_indexes[i],
                    &counter_values[i]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "AGM %d collected byte counter %d failed, rv = %d.\n"),
                    agm_id, i, rv));
                goto _free_exit;
            }
            stat_arr[i].bytes = counter_values[i].bytes;
        }
    }

    /* Convert to agm_stat_attr */
    agm_type = AGM_INFO(unit, agm_id).attr.agm_type;
    if (agm_type == bcmSwitchAgmTypeTrunk ||
        agm_type == bcmSwitchAgmTypeFabricTrunk) {
        rv = _th_agm_trunk_member_fill(unit, agm_id,
                    (bcm_trunk_t)AGM_INFO(unit, agm_id).group_id,
                    nstat, stat_arr);
    } else if ((agm_type == bcmSwitchAgmTypeL3Ecmp) ||
               (agm_type == bcmSwitchAgmTypeL3EcmpOverlay)) {
#if defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_l3)) {
            rv = _th_agm_l3_ecmp_member_fill(unit, agm_id,
                        (bcm_if_t)AGM_INFO(unit, agm_id).group_id,
                        nstat, stat_arr);
        } else
#endif
        rv = BCM_E_UNAVAIL;
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d group member fill failed, group_id %d, rv = %d.\n"), agm_id,
            AGM_INFO(unit, agm_id).group_id, rv));
    }

_free_exit:
    AGM_UNLOCK(unit);
    if (counter_indexes) {
        sal_free(counter_indexes);
    }

    if (counter_values) {
        sal_free(counter_values);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_stat_clear
 * Purpose:
 *      Clear stats of counter entries attached to the aggregation monitor (id)
 */
int
bcm_th_switch_agm_stat_clear(int unit, bcm_switch_agm_id_t agm_id)
{
    int     i, rv = BCM_E_NONE;
    int     group_size, group_num;
    uint32  *counter_indexes = NULL;
    bcm_stat_value_t *counter_values = NULL;
    int     nstat;

    uint32  num_of_tables = 0;
    int     byte_flag;
    bcm_stat_flex_direction_t   direction = bcmStatFlexDirectionIngress;
    bcm_stat_flex_table_info_t  table_info[BCM_STAT_FLEX_COUNTER_MAX_DIRECTION];

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (!AGM_INFO(unit, agm_id).in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    group_size = AGM_INFO(unit, agm_id).attr.num_members;
    group_num  = AGM_INFO(unit, agm_id).attr.period_num;
    nstat = group_size * (group_num + 1);
    AGM_UNLOCK(unit);

    counter_indexes = sal_alloc(sizeof(uint32) * nstat, "cntidx");
    if (counter_indexes == NULL) {
        return BCM_E_MEMORY;
    }
    counter_values = sal_alloc(sizeof(bcm_stat_value_t) * nstat, "cntval");
    if (counter_values == NULL) {
        sal_free(counter_indexes);
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        counter_indexes[i] = i;
    }
    sal_memset(counter_values, 0, sizeof(bcm_stat_value_t) * nstat);

    rv = bcm_th_agm_stat_get_table_info(
            unit, agm_id, &num_of_tables, &table_info[0]);
    if (rv < 0) {
        goto _free_exit;
    }

    if (table_info[0].direction == direction) {
        /* Clear packet counters */
        byte_flag = 0;
        for (i = 0; i < nstat; i++) {
            rv = _bcm_esw_stat_counter_set(unit, table_info[0].index,
                    table_info[0].table, 0, byte_flag, counter_indexes[i],
                    &counter_values[i]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "AGM %d clear packet counter %d failed, rv = %d.\n"),
                    agm_id, i, rv));
                goto _free_exit;
            }
        }

        /* Clear byte counters */
        byte_flag = 1;
        for (i = 0; i < nstat; i++) {
            rv = _bcm_esw_stat_counter_set(unit, table_info[0].index,
                    table_info[0].table, 0, byte_flag, counter_indexes[i],
                    &counter_values[i]);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                    "AGM %d clear byte counter %d failed, rv = %d.\n"),
                    agm_id, i, rv));
                goto _free_exit;
            }
        }
    }

_free_exit:
    if (counter_indexes) {
        sal_free(counter_indexes);
    }

    if (counter_values) {
        sal_free(counter_values);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_destroy
 * Purpose:
 *      Destroy an existing aggregation monitoring entry
 */
int
bcm_th_switch_agm_destroy(int unit, bcm_switch_agm_id_t agm_id)
{
    int i, rv = BCM_E_NONE;
    agm_monitor_t *agm_mnt;
    int pool_id;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    agm_mnt = &AGM_INFO(unit, agm_id);
    if (agm_mnt->attr.agm_id == _AGM_ID_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    /* Disable running AGM */
    AGM_LOCK(unit);
    if (!agm_mnt->in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    if (agm_mnt->in_use && agm_mnt->group_id != _AGM_GROUP_ID_INVALID) {
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d is attached need to detach first\n"), agm_id));
        return BCM_E_BUSY;
    }

    if (agm_mnt->running) {
        rv = bcm_th_switch_agm_enable_set(unit, agm_id, FALSE);
        if (BCM_FAILURE(rv)) {
            AGM_UNLOCK(unit);
            LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
                "AGM %d disable failed, rv = %d.\n"), agm_id, rv));
            return rv;
        }
    }

    /* Destory flex counter */
    rv = bcm_th_agm_stat_detach(unit, agm_mnt->attr.agm_id,
                agm_mnt->agm_pool_id, (uint32)agm_mnt->counter_id);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d detach failed, rv = %d.\n"), agm_id, rv));
        return rv;
    }

    rv = bcm_th_agm_stat_id_clear(unit, agm_mnt->counter_id);
    if (BCM_FAILURE(rv)) {
        AGM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d destroy counter id failed, rv = %d.\n"), agm_id, rv));
        return rv;
    }

    /* Restore AGM bookeeping info to default value */
    pool_id = agm_mnt->agm_pool_id;
    agm_mnt->in_use         = FALSE;
    agm_mnt->running        = FALSE;
    agm_mnt->agm_pool_id    = _AGM_POOL_ID_INVALID;
    agm_mnt->group_id       = _AGM_GROUP_ID_INVALID;
    agm_mnt->counter_id     = 0;
    sal_memset(&agm_mnt->attr, 0, sizeof(agm_mnt->attr));
    agm_mnt->attr.agm_id    = _AGM_ID_INVALID;

    rv = WRITE_AGM_MONITOR_TABLEm(unit, MEM_BLOCK_ANY, agm_id,
            soc_mem_entry_null(unit, AGM_MONITOR_TABLEm));
    if (BCM_SUCCESS(rv)) {
        /* Using ref_cnt? */
        for (i = AGM_CNTL(unit).agm_id_min; i <= AGM_CNTL(unit).agm_id_max; i++) {
            if (AGM_INFO(unit, i).in_use &&
                AGM_INFO(unit, i).agm_pool_id == pool_id) {
                break;
            }
        }

        /* Free pool_id if nobody use it */
        if (i > AGM_CNTL(unit).agm_id_max) {
            AGM_CNTL(unit).agm_pool_conf[pool_id] = _AGM_POOL_ID_INVALID;
        }
    }

    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_trunk_attach_get
 * Purpose:
 *      Retrieve a (fabric) trunk_id where the monitor entry is attached
 */
int
bcm_th_switch_agm_trunk_attach_get(int unit, bcm_switch_agm_id_t agm_id,
                                   bcm_trunk_t *trunk_id)
{
    int rv = BCM_E_NONE;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (trunk_id == NULL) {
        return BCM_E_PARAM;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);

    if (!AGM_INFO(unit, agm_id).in_use ||
        AGM_INFO(unit, agm_id).group_id == _AGM_GROUP_ID_INVALID ||
        (AGM_INFO(unit, agm_id).attr.agm_type != bcmSwitchAgmTypeTrunk &&
         AGM_INFO(unit, agm_id).attr.agm_type != bcmSwitchAgmTypeFabricTrunk)
        ){
        AGM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d trunk attach not found\n"), agm_id));
        return BCM_E_NOT_FOUND;
    }

    *trunk_id = (bcm_trunk_t)AGM_INFO(unit, agm_id).group_id;

    AGM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_l3_ecmp_attach_get
 * Purpose:
 *      Retrieve a l3 ecmp group id where the monitor entry is attached
 */
int
bcm_th_switch_agm_l3_ecmp_attach_get(int unit, bcm_switch_agm_id_t agm_id,
                                     bcm_if_t *l3_ecmp_id)
{
    int rv = BCM_E_NONE;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (l3_ecmp_id == NULL) {
        return BCM_E_PARAM;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (!AGM_INFO(unit, agm_id).in_use ||
        AGM_INFO(unit, agm_id).group_id == _AGM_GROUP_ID_INVALID ||
        ((AGM_INFO(unit, agm_id).attr.agm_type != bcmSwitchAgmTypeL3Ecmp) &&
         (AGM_INFO(unit, agm_id).attr.agm_type != bcmSwitchAgmTypeL3EcmpOverlay))) {
        AGM_UNLOCK(unit);
        LOG_ERROR(BSL_LS_BCM_SWITCH, (BSL_META_U(unit,
            "AGM %d l3 ecmp attach not found\n"), agm_id));
        return BCM_E_NOT_FOUND;
    }

    *l3_ecmp_id = (bcm_if_t)AGM_INFO(unit, agm_id).group_id;

    AGM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_get
 * Purpose:
 *      Retrieve the AGM monitor info of the specified AGM id.
 */
int
bcm_th_switch_agm_get(int unit, bcm_switch_agm_info_t *agm_info)
{
    int rv = BCM_E_NONE;
    bcm_switch_agm_id_t agm_id;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (agm_info == NULL) {
        return BCM_E_PARAM;
    }

    AGM_INIT(unit);
    agm_id = agm_info->agm_id;
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (!AGM_INFO(unit, agm_id).in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    sal_memcpy(agm_info, &AGM_INFO(unit, agm_id).attr, sizeof(*agm_info));

    AGM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_enable_get
 * Purpose:
 *      Return a monitor entry's enable status
 */
int
bcm_th_switch_agm_enable_get(int unit, bcm_switch_agm_id_t agm_id, int *enable)
{
    int rv = BCM_E_NONE;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (enable == NULL) {
        return BCM_E_PARAM;
    }

    AGM_INIT(unit);
    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (!AGM_INFO(unit, agm_id).in_use) {
        AGM_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }
    *enable = AGM_INFO(unit, agm_id).running;

    AGM_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_switch_agm_traverse
 * Purpose:
 *      Traverse over the aggregation monitor table (63 total entries)
 *      and run callback at each entry.
 */
int
bcm_th_switch_agm_traverse(int unit, int flags,
                           bcm_switch_agm_traverse_cb trav_fn,
                           void *user_data)
{
    int i, rv = BCM_E_NONE;

    /* Validate parameters */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    AGM_INIT(unit);

    if (trav_fn == NULL) {
        return BCM_E_PARAM;
    }

    /* Traverse each AGM in use */
    AGM_LOCK(unit);
    for (i = AGM_CNTL(unit).agm_id_min; i <= AGM_CNTL(unit).agm_id_max; i++) {
        if (!AGM_INFO(unit, i).in_use) {
            continue;
        }

        rv = (*trav_fn)(unit, &AGM_INFO(unit, i).attr, user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    AGM_UNLOCK(unit);

    return rv;
}

int
bcm_th_switch_agm_info(int unit, bcm_switch_agm_id_t agm_id,
                       agm_monitor_t *agm_mnt)
{
    int rv = BCM_E_NONE;

    if (agm_mnt == NULL) {
        return BCM_E_PARAM;
    }

    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    rv = _bcm_th_switch_agm_info(unit, agm_id, agm_mnt);
    AGM_UNLOCK(unit);

    return rv;
}

int
bcm_th_switch_agm_fwd_grp_update(int unit, bcm_switch_agm_id_t agm_id,
                                 int group_id)
{
    int rv = BCM_E_NONE;

    AGM_CHECK(unit, agm_id);

    AGM_LOCK(unit);
    if (AGM_INFO(unit, agm_id).in_use) {
        AGM_INFO(unit, agm_id).group_id = group_id;
    } else {
        rv = BCM_E_NOT_FOUND;
    }
    AGM_UNLOCK(unit);

    return rv;
}

int
bcm_th_switch_agm_id_get_by_group(int unit, int group_id,
                                  bcm_switch_agm_id_t *agm_id)
{
    int i;

    if (agm_id == NULL) {
        return BCM_E_PARAM;
    }

    AGM_LOCK(unit);
    for (i = AGM_CNTL(unit).agm_id_min; i <= AGM_CNTL(unit).agm_id_max; i++) {
        if (AGM_INFO(unit, i).in_use &&
            AGM_INFO(unit, i).group_id == group_id) {
            *agm_id = i;
            AGM_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }
    AGM_UNLOCK(unit);

    return BCM_E_NOT_FOUND;
}
/*
 * Update following registers/field
 *     IFP_METER_CONTROLr, PACKET_IFG_BYTESf
 *     EGR_COUNTER_CONTROLr, PACKET_IFG_BYTESf
 *     EGR_COUNTER_CONTROLr, PACKET_IFG_BYTES_2f
 *     EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf
 *     EGR_SHAPING_CONTROLr, PACKET_IFG_BYTES_2f
 */
int
bcm_th_xgs3_meter_adjust_set(int unit, bcm_port_t port, int arg)
{
    int len, max_val;
    int i;
    soc_reg_t reg[3] = { IFP_METER_CONTROLr,
                         EGR_COUNTER_CONTROLr,
                         EGR_SHAPING_CONTROLr };

    if (!soc_feature(unit, soc_feature_meter_adjust)) {
        return BCM_E_UNAVAIL;
    }

    for (i = 0; i < 3 ; ++i ) { 
        if (SOC_REG_FIELD_VALID(unit, reg[i], PACKET_IFG_BYTESf)) {
            len = soc_reg_field_length(unit, reg[i], PACKET_IFG_BYTESf);
            max_val = (1 << len) - 1;
            if (arg < 0 || arg > max_val) {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, reg[i], port,
                                       PACKET_IFG_BYTESf, arg));
        }

        if (SOC_REG_FIELD_VALID(unit, reg[i], PACKET_IFG_BYTES_2f)) {
            len = soc_reg_field_length(unit, reg[i], PACKET_IFG_BYTES_2f);
            max_val = (1 << len) - 1;
            if (arg < 0 || arg > max_val) {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(
                soc_reg_field32_modify(unit, reg[i], port,
                                       PACKET_IFG_BYTES_2f, arg));
        }
    }

    return BCM_E_NONE;
}

int
bcm_th_xgs3_meter_adjust_get(int unit, bcm_port_t port, int *arg)
{
    uint32 val;

    BCM_IF_ERROR_RETURN(
        READ_EGR_COUNTER_CONTROLr(unit, port, &val));
    *arg = soc_reg_field_get(unit, EGR_COUNTER_CONTROLr, val,
                               PACKET_IFG_BYTESf);
    return BCM_E_NONE;
}


#endif  /* defined(BCM_TOMAHAWK_SUPPORT) */

