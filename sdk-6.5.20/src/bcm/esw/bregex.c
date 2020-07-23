/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to program Regex engine for flow tracking
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#if defined(INCLUDE_REGEX)

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/fb4regex.h>
#include <bcm_int/esw/bregex.h>
#include <bcm/bregex.h>
#include <bcm_int/esw_dispatch.h>

STATIC _bcm_esw_regex_device_info_t *_regex_info[BCM_MAX_NUM_UNITS] = {0};

#define ESW_REGEX_INFO(_unit)   _regex_info[_unit]
#define ESW_REGEX_LOCK(_unit)   _regex_lock(_unit)
#define ESW_REGEX_UNLOCK(_unit) _regex_unlock(_unit)

/* This macro will return one less than the physical number of policies because the first
 * policy entry is reserved for the default policy. */
#define ESW_REGEX_POLICIES_MAX(_unit)   (ESW_REGEX_INFO(_unit)->policy_index_max - ESW_REGEX_INFO(_unit)->policy_index_min)
/* This macro will return one more than the first index since the first entry is reserved
 * for the default policy. */
#define ESW_REGEX_POLICIES_FIRST(_unit) (ESW_REGEX_INFO(_unit)->policy_index_min + 1)
#define ESW_REGEX_POLICIES_LAST(_unit)  (ESW_REGEX_INFO(_unit)->policy_index_max)

#define IS_FLOW_MODE(f_pl)                          \
    ((f_pl)->cfg.mode == bcmPolicerModeCommitted || \
     (f_pl)->cfg.mode == bcmPolicerModePeak)

#define IS_MOD_TRTCM_MODE(f_pl)                                 \
    ((f_pl)->cfg.mode == bcmPolicerModeTrTcmDs ||               \
     (f_pl)->cfg.mode == bcmPolicerModeCoupledTrTcmDs)

STATIC int last_allocated_policy = 0;

STATIC int _regex_policy_create_id(int unit, bcm_regex_policy_t policy);
STATIC int _regex_policy_install(int unit, bcm_regex_policy_t policy);

STATIC int _regex_lock(int unit)
{
    _bcm_esw_regex_device_info_t *device;

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    sal_mutex_take(device->regex_esw_mutex, sal_mutex_FOREVER);
    return BCM_E_NONE;
}

STATIC void _regex_unlock(int unit)
{
    _bcm_esw_regex_device_info_t *device;

    device = ESW_REGEX_INFO(unit);
    sal_mutex_give(device->regex_esw_mutex);
}

void _bcm_esw_regex_policy_action_t_init(_regex_policy_action_t *pa)
{
    sal_memset(pa, 0, sizeof(*pa));
    pa->hw_index  = _REGEX_INVALID_INDEX;
    pa->old_index = _REGEX_INVALID_INDEX;
}

void _bcm_esw_regex_policy_t_init(_regex_policy_t *policy)
{
    int idx;

    sal_memset(policy, 0, sizeof(*policy));

    policy->statistic.sid = _REGEX_INVALID_INDEX;
    /* Initialize policer ids as invalid for all policer levels. */
    for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
        policy->policer[idx].pid = _REGEX_INVALID_INDEX;
    }
}

void _bcm_esw_regex_device_info_t_init(_bcm_esw_regex_device_info_t *info)
{
    sal_memset(info, 0, sizeof(*info));
}

/*
 * Function:
 *     _regex_policy_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
STATIC char *_regex_policy_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    static char *action_text[] = BCM_FIELD_ACTION_STRINGS;
    assert(COUNTOF(action_text) == bcmFieldActionCount);

    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

/*
 * Function:
 *     _bcm_regex_policy_action_dest_check
 * Purpose:
 *     Verify destination prameters for fp actions
 * Parameters:
 *     unit    - (IN) BCM device number
 *     fa      - (IN) Field action structure.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_regex_policy_action_dest_check(int unit, _regex_policy_action_t *pa)
{
    int port_out;     /* Calculated port value.    */
    int modid_out;    /* Calculated module id.     */
    int rv;           /* Operation return value.   */

    /* Input parameters check. */
    if (NULL == pa) {
        return (BCM_E_PARAM);
    }

    /* No checks for Mirror/Mpls/Mim/Wlan gports  */
    if (BCM_GPORT_IS_SET(pa->param[1])) {
        return (BCM_E_NONE);
    }

    /* Modport/Trunk were resolved in _field_params_api_to_hw_adapt call.*/
    if (bcmFieldActionRedirectTrunk == pa->action) {
        return  _bcm_trunk_id_validate(unit, pa->param[0]);
    }

    /* Check destination module id / port. */
    rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                pa->param[0], pa->param[1],
                                &modid_out, &port_out);
    BCM_IF_ERROR_RETURN(rv);

    if (!SOC_MODID_ADDRESSABLE(unit, modid_out)) {
        return (BCM_E_PARAM);
    }
    if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
        return (BCM_E_PORT);
    }
    return (BCM_E_NONE);
}

int bcm_esw_regex_config_set(int unit, bcm_regex_config_t *config)
{
    _bcm_esw_regex_device_info_t *device;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    device->flags = config->flags;
    device->report_buffer_size = config->report_buffer_size;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_config_set(unit, config);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_config_get(int unit, bcm_regex_config_t *config)
{
    _bcm_esw_regex_device_info_t *device;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (NULL == config) {
        return BCM_E_PARAM;
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    config->flags = device->flags;
    config->report_buffer_size = device->report_buffer_size;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_config_get(unit, config);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_exclude_add(int unit, uint8 protocol, 
                            uint16 l4_start, uint16 l4_end)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_exclude_add(unit, protocol, l4_start, l4_end);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_exclude_delete(int unit, uint8 protocol, 
                            uint16 l4_start, uint16 l4_end)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_exclude_delete(unit, protocol, l4_start, l4_end);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_exclude_delete_all(int unit)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_exclude_delete_all(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_exclude_get(int unit, int array_size, uint8 *protocol, 
                                uint16 *l4low, uint16 *l4high, int *array_count)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (!(array_size && protocol && l4low && l4high && array_count)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_exclude_get(unit, array_size, protocol,
                                         l4low, l4high, array_count);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_engine_create(int unit, bcm_regex_engine_config_t *config, 
                            bcm_regex_engine_t *engid)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_engine_create(unit, config, engid);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_engine_destroy(int unit, bcm_regex_engine_t engid)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_engine_destroy(unit, engid);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_engine_traverse(int unit, bcm_regex_engine_traverse_cb cb, 
                                void *user_data)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_engine_traverse(unit, cb, user_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_engine_get(int unit, bcm_regex_engine_t engid, 
                            bcm_regex_engine_config_t *config)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_engine_get(unit, engid, config);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_match_check(int unit, bcm_regex_match_t* match, 
                                int count, int* metric)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_match_check(unit, match, count, metric);

    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int _regex_policy_get(int unit, bcm_regex_policy_t policy, _regex_policy_t **policy_p);

int bcm_esw_regex_match_set(int unit, bcm_regex_engine_t engid,
                            bcm_regex_match_t* match, int count)
{
    _bcm_esw_regex_device_info_t *device;

#if defined(BCM_TRIUMPH3_SUPPORT)
    int i, rv;
    _regex_policy_t *p_policy;
#endif

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        if (count > 0) {
            for (i = 0; i < count; i++) {
                if (match[i].action_flags & BCM_REGEX_MATCH_ACTION_NON_POLICY_ACTIONS) {
                    if (0 != (device->flags & BCM_REGEX_USE_POLICY_ID)) {
                        return BCM_E_PARAM;
                    }
                    if (match[i].policy_id != -1) {
                        return BCM_E_PARAM;
                    }
                }
                else
                {
                    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
                        return BCM_E_PARAM;
                    }
                    if (match[i].policy_id == -1) {
                        return BCM_E_PARAM;
                    }
                    rv = _regex_policy_get(unit, match[i].policy_id, &p_policy);
                    BCM_IF_ERROR_RETURN(rv);

                    if ((p_policy->flags & _REGEX_POLICY_INSTALLED) == 0) {
                        return BCM_E_PARAM;
                    }
                }
            }
        }

        return _bcm_tr3_regex_match_set(unit, engid, match, count);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}


int bcm_esw_regex_get_match_id(int unit, int signature_id, int *match_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_get_match_id(unit, signature_id, match_id);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}


int bcm_esw_regex_get_sig_id(int unit, int match_id, int *signature_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_get_sig_id(unit, match_id, signature_id);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

#define BCM_REGEX_NUM_SLOT_PER_CPS 16

typedef struct _bcm_regex_cps_obj_pub_s {
    uint32    flags;
    int             foff;
    int             alloc_map[BCM_REGEX_NUM_SLOT_PER_CPS];
    int             refcnt[BCM_REGEX_NUM_SLOT_PER_CPS];
    int             from_line;
    int             req_lsz;
} _bcm_regex_cps_obj_pub_t;

int bcm_esw_regex_engine_info_get(int unit, int engine_id,
                              bcm_regex_engine_info_t *engine_info)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_get_engine_size_info(unit, engine_id, engine_info);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}


int bcm_esw_regex_info_get(int unit, bcm_regex_info_t *regex_info)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    _bcm_esw_regex_device_info_t *device;
#endif

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        /*
        Assume we are using the new policy tables.  If not,
        these values will be overwritten by the old values.
        */
        regex_info->policy_size = ESW_REGEX_POLICIES_MAX(unit) + 1;
        device = ESW_REGEX_INFO(unit);
        regex_info->policy_free_size = regex_info->policy_size - device->num_policies;

        return _bcm_tr3_regex_info_get(unit, regex_info);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}


int bcm_esw_regex_report_register(int unit, uint32 reports, 
                                  bcm_regex_report_cb callback, void *user_data)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_esw_regex_report_register(unit, reports, 
                                             callback, user_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int  bcm_esw_regex_stat_get(int unit, bcm_regex_stat_t type, uint64 *val)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_stat_get(unit, type, val);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

int  bcm_esw_regex_stat_set(int unit, bcm_regex_stat_t type, uint64 val)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_stat_set(unit, type, val);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_report_unregister(int unit, uint32 reports, 
                                bcm_regex_report_cb callback, void *user_data)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_esw_regex_report_unregister(unit, reports, 
                                               callback, user_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return BCM_E_UNAVAIL;
}

int bcm_esw_regex_init(int unit)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_esw_regex_device_info_t *info;
    _field_stage_t               *field_stage;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    last_allocated_policy = 0;
    /* Init regex info.  */
    info = ESW_REGEX_INFO(unit);
    if (NULL == info) {
        info = sal_alloc(sizeof(*info), "_regex_info");
        if (NULL == info) {
            return BCM_E_MEMORY;
        }
        ESW_REGEX_INFO(unit) = info;
        _bcm_esw_regex_device_info_t_init(info);
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_tr3_regex_init(unit, &info->functions);
    }
#endif
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    info->regex_esw_mutex = sal_mutex_create("regex_esw_mutex");

    info->policy_index_min = info->functions->policy_index_min_get(unit);
    info->policy_index_max = info->functions->policy_index_max_get(unit);

    /* The last meter pool is reserved for use by the regex component so the Field
     * Processor will only have (total pools - 1).  Calculate the starting HW index of the
     * last pool and ensure that it is valid. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &field_stage));
    info->meter_base_hw_index = field_stage->num_meter_pools * field_stage->meter_pool[_FP_DEF_INST][0]->pool_size;
    info->meter_pool = field_stage->num_meter_pools;
    assert(soc_mem_index_count(unit, FP_METER_TABLEm) > info->meter_base_hw_index);

    /*
     * Initialize default policy (which does nothing):
     * _bcm_tr3_regex_init creates a empty policy entry 0 (by calling legacy function
     * _bcm_tr3_install_action_policy).  Initialize the policy 0 data here.
     */
    _regex_policy_create_id(unit, _REGEX_DEFAULT_POLICY_ID);
    _regex_policy_install(unit, _REGEX_DEFAULT_POLICY_ID);

    return BCM_E_NONE;
}

int _bcm_esw_regex_sync(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) && soc_feature(unit, soc_feature_regex)) {
        return _bcm_esw_tr3_regex_sync(unit);
    }
#endif
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _regex_policy_dirty
 * Purpose:
 *     Check if policy was modified after last installation
 * Parameters:
 *     unit        - (IN)  BCM device number.
 *     p_ent       - (IN)  Regex policy pointer.
 *     dirty       - (OUT) Entry dirty flag.
 * Returns:
 *     BCM_E_XXX
 */
int _regex_policy_dirty(int unit, _regex_policy_t *p_ent, int *dirty)
{
    _field_entry_policer_t *f_ent_pl;   /* Field entry policer structure.*/
    _field_policer_t       *f_pl;       /* Field policer descriptor.     */
    int                     rv;         /* Operation return status.      */
    int                     idx;

    /* Input parameters check. */
    if ((NULL == p_ent) || (NULL == dirty)) {
        return (BCM_E_PARAM);
    }

    *dirty = (p_ent->flags & _REGEX_POLICY_DIRTY) ? TRUE : FALSE;

    if (0 == (*dirty)) {
        /* Policer configuration updates check. */
        for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
            f_ent_pl = p_ent->policer + idx;
            /* Skip invalid policers. */
            if (0 == (f_ent_pl->flags & _FP_POLICER_VALID)) {
                continue;
            }
            /* Read policer configuration.*/
            rv = _bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl);
            BCM_IF_ERROR_RETURN(rv);

            /* Check if policer was modified. */
            if (f_pl->hw_flags & _FP_POLICER_DIRTY) {
                *dirty = TRUE;
                break;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _regex_policy_t_compare
 * Purpose:
 *      Compare entry id in _regex_policy_t structure.
 * Parameters:
 *      b - (IN) first  compared qualifier.
 *      a - (IN) second compared qualifier.
 * Returns:
 *      a<=>b
 */
STATIC int _regex_policy_t_compare(void *a, void *b)
{
    _regex_policy_t **first;     /* First compared entry.  */
    _regex_policy_t **second;    /* Second compared entry. */

    first =  (_regex_policy_t **)a;
    second = (_regex_policy_t **)b;

    if ((*first)->eid < (*second)->eid) {
        return (-1);
    } else if ((*first)->eid > (*second)->eid) {
        return (1);
    }
    return (0);
}

/*
 * Function:
 *     _regex_policy_get
 * Purpose:
 *     Lookup a policy (entry) from a unit ID and policy id choice.
 * Parmeters:
 *     unit     - (IN) BCM device number.
 *     policy   - (IN) Policy id.
 *     policy_p - (OUT) Entry lookup result.
 * Returns:
 *     BCM_E_XXX
 */
int _regex_policy_get(int unit, bcm_regex_policy_t policy, _regex_policy_t **policy_p)
{
    _regex_policy_t               target;         /* Entry lookup pointer.    */
    _regex_policy_t              *p_ent;          /* Entry lookup pointer.    */
    _bcm_esw_regex_device_info_t *device;         /* Regex control structure. */
    int                           idx;            /* Entry index.             */

    /* Reset target entry */
    target.eid = policy;
    p_ent = &target;

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (NULL == device->policies) {
        return (BCM_E_NOT_FOUND);
    }

    ESW_REGEX_LOCK(unit);

    idx = _shr_bsearch(device->policies, device->num_policies,
                       sizeof(device->policies[0]), &p_ent,
                       _regex_policy_t_compare);
    ESW_REGEX_UNLOCK(unit);

    if (idx >= 0) {
        if (NULL != policy_p) {
            *policy_p = device->policies[idx];
        }
        return (BCM_E_NONE);
    }

    /* Rule with id == eid not found. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _regex_policy_add
 * Purpose:
 *     Insert an entry to info entry array.
 * Parmeters:
 *     unit    - (IN) BCM device number.
 *     policy  - (IN) Policy entry.
 * Returns:
 *     BCM_E_XXX
 */
int _regex_policy_add(int unit, _regex_policy_t *policy)
{
    int                            idx;    /* Entry insertion index.  */
    _bcm_esw_regex_device_info_t  *device; /* Regex control structure. */
    _regex_policy_t              **policy_array;

    /* Input parameters check */
    if (NULL == policy) {
        return (BCM_E_PARAM);
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Verify if entry already present in the group . */
    if (BCM_E_NONE == _regex_policy_get(unit, policy->eid, NULL)) {
        /* Entry already IN. */
        return (BCM_E_NONE);
    }

    /* Check if group has enough room for new entry */
    if (ESW_REGEX_POLICIES_MAX(unit) > device->num_policies) {

        policy_array = sal_alloc(sizeof(policy_array[0]) * (device->num_policies + 1), "regex_policy_array");
        if (NULL == policy_array) {
            return (BCM_E_MEMORY);
        }

        /* Copy original array to newly allocated one. */
        if (NULL != device->policies) {
            sal_memcpy(policy_array, device->policies, sizeof(policy_array[0]) * device->num_policies);
            sal_free(device->policies);
        }

        device->policies = policy_array;
        device->num_policies++;
    } else {
        return (BCM_E_RESOURCE);
    }

    /* Insert entry in sorted order.  Note that if the loop breaks, idx will be one less
     * than the index at which to add the new policy.  If the loop runs to completion, idx
     * will be -1 so the new entry will be inserted at entry 0 (idx + 1) which is
     * appropriate since the new entry is less than all the other entries. */
    idx = device->num_policies - 2;
    if (idx >= 0) {
        do {
            if (_regex_policy_t_compare(&device->policies[idx], &policy) < 0) {
                break;
            }

            device->policies[idx + 1] = device->policies[idx];
            idx--;
        } while (idx >= 0);
        device->policies[idx + 1] = policy;
    } else {
        device->policies[0] = policy;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_delete
 * Purpose:
 *     Remove an entry from field group entry array.
 * Parmeters:
 *     unit    - (IN)BCM device number.
 *     fg      - (IN)Field group structure.
 *     f_ent   - (IN)Removed entry structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_delete(int unit, _regex_policy_t *policy)
{
    int                            idx;    /* Entry insertion index.  */
    _bcm_esw_regex_device_info_t  *device; /* Regex control structure. */

    /* Input parameters check */
    if (NULL == policy) {
        return (BCM_E_PARAM);
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Make sure policy array was not deallocated. */
    if (NULL == device->policies) {
        return (BCM_E_INTERNAL);
    }

    /* Verify if entry already present in the group . */
    ESW_REGEX_LOCK(unit);
    idx = _shr_bsearch(device->policies, device->num_policies,
                       sizeof(device->policies[0]), &policy,
                       _regex_policy_t_compare);
    if (idx < 0) {
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_NOT_FOUND);
    }

    if (device->num_policies <= 1) {
        sal_free(device->policies);
        device->policies = NULL;
        device->num_policies = 0;
    } else {
        /* Make room for inserted entry */
        while (idx < device->num_policies - 1) {
            device->policies[idx] = device->policies[idx + 1];
            idx++;
        }
        device->num_policies--;
        device->policies[device->num_policies] = NULL;
    }

    ESW_REGEX_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_params_hw_to_api_adapt
 *
 * Purpose:
 *     Adapts action parameters from HW to API space.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     action - (IN)Action to adapt parameters for (bcmFieldActionXXX)
 *     param0 - (IN/OUT)Action parameter
 *     param1 - (IN/OUT)Action parameter
 *
 * Returns:
 *     BCM_E_NONE      - No errors.
 *     BCM_E_XXX       - Otherwise.
 */
STATIC int _regex_policy_params_hw_to_api_adapt(int unit, bcm_field_action_t action,
                                                uint32 *param0, uint32 *param1)
{
    int                 isGport;
    _bcm_gport_dest_t   dest;

    _bcm_gport_dest_t_init(&dest);
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    switch (action) {
        case bcmFieldActionRedirect:
            if (isGport) {
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                dest.modid = *param0;
                dest.port = *param1;
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, (bcm_gport_t *)param1));
                *param0 = -1;   /* If gport requested param0 should be ignored */
            }
            break;
        case bcmFieldActionRedirectTrunk:
            if (isGport) {
                dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
                dest.tgid = *param0;
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, (bcm_gport_t *)param0));
                *param1 = -1;   /* If gport requested param1 should be ignored */
            }
            break;
        default:
            break;
    }
    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_params_api_to_hw_adapt
 *
 * Purpose:
 *     Adapts action parameters from API to HW space.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     action - (IN)Action to adapt parameters for (bcmFieldActionXXX)
 *     param0 - (IN/OUT)Action parameter
 *     param1 - (IN/OUT)Action parameter
 *
 * Returns:
 *     BCM_E_NONE      - No errors.
 *     BCM_E_XXX       - Otherwise.
 */
STATIC int _regex_policy_params_api_to_hw_adapt(int unit, bcm_field_action_t action,
                                                uint32 *param0, uint32 *param1)
{
    bcm_module_t        l_modid;
    bcm_trunk_t         l_tgid;
    bcm_port_t          l_port;
    int                 id;

    switch (action) {
        case bcmFieldActionRedirect:
            if (BCM_GPORT_IS_SET(*param1)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, *param1, &l_modid, &l_port, &l_tgid, &id));
                /* Do not modify param0 and param1 if id != -1 */
                if (-1 == id) {
                    if (BCM_TRUNK_INVALID != l_tgid) {
                        return BCM_E_PARAM;
                    }
                    *param1 = l_port;
                    *param0 = l_modid;
                }
            }
            break;
        case bcmFieldActionRedirectTrunk:
            if (BCM_GPORT_IS_SET(*param0)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, *param0, &l_modid, &l_port, &l_tgid, &id));
                if ((-1 != id) || (BCM_TRUNK_INVALID == l_tgid) ){
                    return BCM_E_PARAM;
                }
                *param0 = l_tgid;
                *param1 = -1;   /* if param0 is trunk, param1 ignored */
            }
            break;
        default:
            break;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_phys_create
 * Purpose:
 *     Initialize a physical policy structure.
 * Parameters:
 *     unit           - (IN) BCM device number.
 *     policy         - (IN) Policy id.
 *     policy_p       - (OUT)Allocated & initialized entry.
 * Returns
 *     BCM_E_XXX
 */
STATIC int _regex_policy_phys_create(int unit, bcm_regex_policy_t policy, _regex_policy_t **policy_p)
{
    int                           rv;         /* Operation return status.         */
    _regex_policy_t              *policy_ent; /* Field entry structure.           */
    _bcm_esw_regex_device_info_t *device;     /* Regex control structure. */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: BEGIN %s(policy=%d)\n"),
               unit, FUNCTION_NAME(), policy));

    if (NULL == policy_p) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Check if free entries are available in the slice. */
    if (device->num_policies >= ESW_REGEX_POLICIES_MAX(unit)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: No room.\n"),
                   unit));
        return (BCM_E_RESOURCE);
    }

    policy_ent = sal_alloc(sizeof(*policy_ent), "regex_policy");
    if (policy_ent == NULL) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: allocation failure for policy_entry\n"),
                   unit));
        return (BCM_E_MEMORY);
    }

    _bcm_esw_regex_policy_t_init(policy_ent);

    /* Fill in entry primitives. */
    policy_ent->eid = policy;

#if 0
    /* Do we want to have a default setting for the FT? */

    /* Enable color independent actions based on field control. */
    if (fc->flags & _FP_COLOR_INDEPENDENT) {
        policy_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
    }
#endif

    /* Mark entry dirty. */
    policy_ent->flags |=  _REGEX_POLICY_DIRTY;

    /* Insert the entry into group entry array. */
    rv = _regex_policy_add(unit, policy_ent);
    if (BCM_FAILURE(rv)) {
        sal_free(policy_ent);
        return (rv);
    }

    /* Return allocated/filled entry structure to the caller. */
    *policy_p = policy_ent;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_phys_destroy
 *
 * Purpose:
 *     Destroy a physical policy. Note that this does not remove
 *     the policy from the hardware.
 *
 * Parameters:
 *     unit      - BCM device number
 *     policy    - Policy ID to remove
 *
 * Returns:
 *     BCM_E_NONE - Success
 */
STATIC int _regex_policy_phys_destroy(int unit, _regex_policy_t *p_ent)
{
    int              rv;          /* Operation return status.        */

    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Remove the entry from group entry array. */
    rv = _regex_policy_delete(unit, p_ent);
    sal_free(p_ent);
    return (rv);
}

/*
 * Function:
 *     _regex_policy_mirror_dest_delete
 * Purpose:
 *     Unset mirroring destination & disable mirroring for rule matching
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     fa       - (IN) Field action descriptor.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_mirror_dest_delete(int unit, _regex_policy_t *p_ent,
                                            _regex_policy_action_t *pa, uint32 flags)
{
    uint32 mirror_flags;      /* Mirror module flags.     */
    int rv;

    /* Input parameters check */
    if ((NULL == pa) || (NULL == p_ent)) {
        return (BCM_E_PARAM);
    }

    /* Set mirror flags. */
    if (bcmFieldActionMirrorIngress == pa->action) {
        mirror_flags = BCM_MIRROR_PORT_INGRESS;
    }  else if (bcmFieldActionMirrorEgress == pa->action) {
        /* Stage Ingress */
        mirror_flags = BCM_MIRROR_PORT_EGRESS;
    } else {
        return (BCM_E_PARAM);
    }

    if ((flags & _REGEX_ACTION_RESOURCE_FREE) &&
        (_REGEX_INVALID_INDEX != pa->hw_index)) {
        rv = _bcm_esw_mirror_fp_dest_delete(unit, pa->hw_index, mirror_flags);
        BCM_IF_ERROR_RETURN(rv);
        pa->hw_index = _REGEX_INVALID_INDEX;
    }

    if ((flags & _REGEX_ACTION_OLD_RESOURCE_FREE) &&
        (_REGEX_INVALID_INDEX != pa->old_index)) {
        rv = _bcm_esw_mirror_fp_dest_delete(unit, pa->old_index, mirror_flags);
        BCM_IF_ERROR_RETURN(rv);
        pa->old_index = _REGEX_INVALID_INDEX;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_mirror_dest_add
 * Purpose:
 *     Set mirroring destination & enable mirroring for rule matching
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     fa       - (IN) field action
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_mirror_dest_add(int unit, _regex_policy_t *p_ent,
                                         _regex_policy_action_t *pa)
{
    uint32 mirror_flags;      /* Mirror module flags.     */

    /* Input parameters check. */
    if ((NULL == pa) || (NULL == p_ent)) {
        return (BCM_E_PARAM);
    }

    /* Set mirror flags. */
    if (bcmFieldActionMirrorIngress == pa->action) {
        mirror_flags = BCM_MIRROR_PORT_INGRESS;
    }  else if (bcmFieldActionMirrorEgress == pa->action) {
        /* Stage Ingress */
        mirror_flags = BCM_MIRROR_PORT_EGRESS;
    } else {
        return (BCM_E_PARAM);
    }

    return _bcm_esw_mirror_fp_dest_add(unit, pa->param[0], pa->param[1],
                                       mirror_flags, &pa->hw_index);
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *     _bcm_regex_policy_set_l3_nh_resolve
 * Purpose:
 *     Install l3 forwarding policy entry.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     value     - (IN) Egress object id or combined next hop information.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_regex_policy_set_l3_nh_resolve(int unit, int value,
                                        uint32 *flags, int *nh_ecmp_id)
{
    int rv;           /* Operation return value.    */

    /* Check if egress forwarding mode is enabled. */
    rv = bcm_xgs3_l3_egress_id_parse(unit, value, flags, nh_ecmp_id);
    if (rv == BCM_E_DISABLED) {
        if (_REGEX_L3_ACTION_UNPACK_ECMP_MODE((value))) {
            *flags = BCM_L3_MULTIPATH;
            *nh_ecmp_id = _REGEX_L3_ACTION_UNPACK_ECMP_PTR((value));
        } else {
            *flags = 0;
            *nh_ecmp_id = _REGEX_L3_ACTION_UNPACK_NEXT_HOP((value));
        }
        rv = BCM_E_NONE;
    }
    return (rv);
}

/*
 * Function:
 *     _regex_policy_l2_actions_nh_destroy
 *
 * Purpose:
 *     Free l3 next hop and egress interface for
 *     l2 fields update action.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     nh_index - (IN) Next hop index.
 *     egr_intf_set - (IN) Egress interface allocated.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_l2_actions_nh_destroy(int unit, int nh_index, int egr_intf_set)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer.            */
    bcm_l3_egress_t egr;                    /* Egress forwarding object.    */
    uint32 intf;                            /* Egress L3 interface id.      */

    /* Initialization. */
    bcm_l3_egress_t_init(&egr);

    /* Read next hop. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, SOC_BLOCK_ANY,
                                     nh_index, hw_buf));

    /* clear egress interface information + free interface resource */
    if (1 == egr_intf_set) {
        /* Get interface id. */
        intf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, hw_buf, INTF_NUMf);
        /* Reset interface entry in hw. */
        sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                                          intf, hw_buf));
        /* Free interface id. */
        BCM_IF_ERROR_RETURN(_bcm_xgs3_egress_l3_intf_id_free(unit, intf));
    }

    /* clear egress next hop table information + free next hop index */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                      SOC_BLOCK_ALL, nh_index, hw_buf));

    BCM_IF_ERROR_RETURN(bcm_xgs3_nh_del(unit, 0, nh_index));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_l2_actions_nh_create
 *
 * Purpose:
 *     Free l3 next hop and egress interface for
 *     l2 fields update action.
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     da_pa   - (IN) Update destination mac action.
 *     sa_pa   - (IN) Update source mac action.
 *     vid_pa  - (IN) Update outer vid action.
 *     vn_new_pa  - (IN) Change VN tag action.
 *     vn_del_pa  - (IN) Delete VN tag action.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_l2_actions_nh_create(int unit,
                                              _regex_policy_action_t *da_pa,
                                              _regex_policy_action_t *sa_pa,
                                              _regex_policy_action_t *vid_pa,
                                              _regex_policy_action_t *vn_new_pa,
                                              _regex_policy_action_t *vn_del_pa)
{
    uint32          hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer.     */
    bcm_l3_egress_t egr;                             /* Egress forwarding object.     */
    _bcm_l3_intf_cfg_t intf;                            /* Egress L3 interface id.       */
    int             nh_index;                        /* Next hop index.               */
    int             nh_flags = 0;                    /* Next hop flags.               */
    int             rv       = BCM_E_NONE;           /* Operation return status.      */

    /* Initialization. */
    bcm_l3_egress_t_init(&egr);
    sal_memset(&intf, 0, sizeof(_bcm_l3_intf_cfg_t));

    /* Extract the policy info from the entry structure. */
    if (NULL != da_pa) {
        SAL_MAC_ADDR_FROM_UINT32(egr.mac_addr, da_pa->param);
    }

    if (NULL != sa_pa) {
        SAL_MAC_ADDR_FROM_UINT32(intf.l3i_mac_addr, sa_pa->param);
    }

    if (NULL != vid_pa) {
        intf.l3i_vid = vid_pa->param[0];
    }

    /* Create egress l3 interface. */
    BCM_IF_ERROR_RETURN(_bcm_xgs3_egress_l3_intf_id_alloc(unit, &intf));

    /* Write egress interface to the hw. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set mac address. */
    soc_mem_mac_addr_set(unit, EGR_L3_INTFm, hw_buf,
                         MAC_ADDRESSf, intf.l3i_mac_addr);

    /* Set vlan id. */
    soc_mem_field32_set(unit, EGR_L3_INTFm, hw_buf, VIDf, intf.l3i_vid);

    /* Write interface configuration to the HW. */
    rv = soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                       intf.l3i_index, hw_buf);
    if (BCM_FAILURE(rv)) {
        (void)_bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3i_index);
        return rv;
    }

    /* Allocate next hop entry. */
    nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
    rv = bcm_xgs3_nh_add(unit, nh_flags, &egr, &nh_index);
    if (BCM_FAILURE(rv)) {
        sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        (void)soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                            intf.l3i_index, hw_buf);
        (void)_bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3i_index);
        return (rv);
    }

    /* Write egress next hop entry. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set next hop mac address. */
    soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                         MAC_ADDRESSf, egr.mac_addr);

    /* Set interface id. */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                        INTF_NUMf, intf.l3i_index);

    /* Set Disable flags. */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                        L3__L3_UC_TTL_DISABLEf, 0x1);

    if (NULL == vid_pa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                            L3__L3_UC_VLAN_DISABLEf, 0x1);
    }

    if (NULL == sa_pa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                            L3__L3_UC_SA_DISABLEf, 0x1);
    }

    if (NULL == da_pa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf,
                            L3__L3_UC_DA_DISABLEf, 0x1);
    }

    if (SOC_IS_TRIUMPH3(unit)) {
        soc_mem_field32_set(unit,
                            EGR_L3_NEXT_HOPm,
                            hw_buf,
                            ENTRY_TYPEf,
                            6);
    }


    /* Insert next hop information. */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, SOC_BLOCK_ALL,
                       nh_index, hw_buf);
    if (BCM_FAILURE(rv)) {
        sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        (void)soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                            intf.l3i_index, hw_buf);
        (void)_bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3i_index);
        return (rv);
    }

    /* Preserve next hop index in the action structure. */
    if (NULL != vid_pa) {
        vid_pa->hw_index = nh_index;
    }

    if (NULL != sa_pa) {
        sa_pa->hw_index = nh_index;
    }

    if (NULL != da_pa) {
        da_pa->hw_index = nh_index;
    }

    if (NULL != vn_new_pa) {
        vn_new_pa->hw_index = nh_index;
    }

    if (NULL != vn_del_pa) {
        vn_del_pa->hw_index = nh_index;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_l2_actions_hw_alloc
 *
 * Purpose:
 *     Allocate l3 next hop and egress interface for
 *     l2 fields update action.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy descriptor.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_l2_actions_hw_alloc(int unit, _regex_policy_t *p_ent)
{
    _regex_policy_action_t *vid_pa;        /* Update outer vid action.      */
    _regex_policy_action_t *sa_pa;         /* Update source mac action.     */
    _regex_policy_action_t *da_pa;         /* Update destination mac action.*/
    _regex_policy_action_t *vn_new_pa;     /* Change VN tag.                */
    _regex_policy_action_t *vn_del_pa;     /* Delete VN tag action.         */
    _regex_policy_action_t *pa;            /* Field action descriptor.      */

    /*
     * Applicable to stage ingress on devices which support
     * soc_feature_field_action_l2_change feature.
     */

    /* Initialization. */
    sa_pa = da_pa = vid_pa = vn_new_pa = vn_del_pa = NULL;

    /* Extract the policy info from the entry structure. */
    for (pa = p_ent->actions; ((pa != NULL) && (_REGEX_ACTION_VALID & pa->flags)); pa = pa->next) {
        switch (pa->action) {
            case bcmFieldActionSrcMacNew:
                if (_REGEX_INVALID_INDEX != pa->hw_index) {
                    pa->old_index = pa->hw_index;
                }
                sa_pa = pa;
                break;
            case bcmFieldActionDstMacNew:
                if (_REGEX_INVALID_INDEX != pa->hw_index) {
                    pa->old_index = pa->hw_index;
                }
                da_pa = pa;
                break;
            case bcmFieldActionOuterVlanNew:
                if (_REGEX_INVALID_INDEX != pa->hw_index) {
                    pa->old_index = pa->hw_index;
                }
                vid_pa = pa;
                break;
            case bcmFieldActionVnTagNew:
                if (_REGEX_INVALID_INDEX != pa->hw_index) {
                    pa->old_index = pa->hw_index;
                }
                vn_new_pa = pa;
                break;
            case bcmFieldActionVnTagDelete:
                if (_REGEX_INVALID_INDEX != pa->hw_index) {
                    pa->old_index = pa->hw_index;
                }
                vn_del_pa = pa;
                break;
            default:
                continue;
        }
    }

    /* Create nh entry. */
    if ((NULL != vid_pa) || (NULL != da_pa) || (NULL != sa_pa) ||
        (NULL != vn_new_pa) || (NULL != vn_del_pa)) {
        BCM_IF_ERROR_RETURN(_regex_policy_l2_actions_nh_create(unit, da_pa, sa_pa,
                                                               vid_pa, vn_new_pa, vn_del_pa));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_l2_actions_hw_free
 *
 * Purpose:
 *     Free l3 next hop and egress interface for
 *     l2 fields update action.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy descriptor.
 *     flags     - (IN) Free flags (old/new/both).
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_l2_actions_hw_free(int unit, _regex_policy_t *p_ent,
                                            uint32 flags)
{
    _regex_policy_action_t *pa;                        /* Field action descriptor.      */
    int                     nh_index;                  /* Next hop index.               */
    int                     old_nh_index;              /* Old next hop index.           */
    int                     egr_intf_set = 0;          /* Egress interface allocated    */
    int                     rv           = BCM_E_NONE; /* Operation return status.      */

    /* Applicable to stage ingress on TRX and FB family of devices only. */

    /* Initialization. */
    nh_index = old_nh_index = _REGEX_INVALID_INDEX;

    /* Extract the policy info from the entry structure. */
    for (pa = p_ent->actions; pa != NULL; pa = pa->next) {
        switch (pa->action) {
            case bcmFieldActionSrcMacNew:
                egr_intf_set = TRUE;
                /* passthru */
                /* coverity[MISSING_BREAK: FALSE] */
            case bcmFieldActionOuterVlanNew:
            case bcmFieldActionDstMacNew:
            case bcmFieldActionVnTagNew:
            case bcmFieldActionVnTagDelete:
                if ((flags & _REGEX_ACTION_RESOURCE_FREE) &&
                    (_REGEX_INVALID_INDEX != pa->hw_index)) {
                    nh_index = pa->hw_index;
                    pa->hw_index = _REGEX_INVALID_INDEX;
                }
                if ((flags & _REGEX_ACTION_OLD_RESOURCE_FREE) &&
                    (_REGEX_INVALID_INDEX != pa->old_index)) {
                    old_nh_index = pa->old_index;
                    pa->old_index = _REGEX_INVALID_INDEX;
                }
                break;
            default:
                break;
        }
    }

    /* Destroy old next hop if any. */
    if (_REGEX_INVALID_INDEX != old_nh_index) {
        rv = _regex_policy_l2_actions_nh_destroy(unit, old_nh_index, egr_intf_set);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (_REGEX_INVALID_INDEX != nh_index) {
        rv = _regex_policy_l2_actions_nh_destroy(unit, nh_index, egr_intf_set);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _regex_policy_action_hw_resources_free
 * Purpose:
 *      Free hw resources allocated for regex policy action installation.
 * Parameters:
 *      unit     - (IN) BCM device number
 *      p_ent    - (IN) Regex policy structure.
 *      pa       - (IN) Policy action structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _regex_policy_action_hw_resources_free(int unit, _regex_policy_t *p_ent,
                                           _regex_policy_action_t *pa, uint32 flags)
{
    _bcm_esw_regex_device_info_t *device;    /* Regex control structure. */
    int rv;

    /* Input parameters check. */
    if ((NULL == p_ent) || (NULL == pa)) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    switch (pa->action) {
      case bcmFieldActionMirrorIngress:
      case bcmFieldActionMirrorEgress:
          return _regex_policy_mirror_dest_delete(unit, p_ent, pa, flags);
          break;
      case bcmFieldActionRedirectMcast:
#if defined(INCLUDE_L3)
      case bcmFieldActionRedirectIpmc:
#endif /* INCLUDE_L3 */
          if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
              break;
          }
      case bcmFieldActionRedirectPbmp:
      case bcmFieldActionEgressMask:
      case bcmFieldActionEgressPortsAdd:
          /* Applicable to stage ingress on TRX devices only. */
          rv = device->functions->redirect_profile_delete(unit, pa->hw_index);
          BCM_IF_ERROR_RETURN(rv);
          break;

#if defined(INCLUDE_L3)
      case bcmFieldActionSrcMacNew:
      case bcmFieldActionDstMacNew:
      case bcmFieldActionOuterVlanNew:
          if (soc_feature(unit, soc_feature_field_action_l2_change)) {
              rv = _regex_policy_l2_actions_hw_free(unit, p_ent,
                                                 _REGEX_ACTION_HW_FREE);
              BCM_IF_ERROR_RETURN(rv);
          }
          break;
#endif /* INCLUDE_L3 */

      default:
          break;
    }
    pa->hw_index = _REGEX_INVALID_INDEX;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _regex_policy_invalid_actions_remove
 * Purpose:
 *      Remove deleted actions from policy actions
 *      linked list.
 * Parameters:
 *      unit  - (IN) BCM device number
 *      p_ent - (IN) Regex policy structure.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _regex_policy_invalid_actions_remove(int unit, _regex_policy_t *p_ent)
{
    _regex_policy_action_t     *pa;
    _regex_policy_action_t     *pa_prev = NULL;
    _regex_policy_action_t     *pa_next = NULL;
    _regex_policy_action_t     *pa_iter;        /* Field entry actions iterator.*/
    int                 rv;

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Find the action in the policy */
    pa = p_ent->actions; /* start at head */
    while (pa != NULL) {
        pa_next = pa->next;
        if (pa->flags & _REGEX_ACTION_VALID) {
            pa_prev = pa;
            pa      = pa_next;
            continue;
        }

        if (_REGEX_INVALID_INDEX != pa->hw_index) {
            /*
             * Check if a valid instance of this action exists in
             * the entry actions linked list.
             * If TRUE, hardware index must be freed only
             * after updating policy table with new information.
             */
            if ((bcmFieldActionEgressMask        == pa->action) ||
                (bcmFieldActionRedirectBcastPbmp == pa->action) ||
                (bcmFieldActionRedirectPbmp      == pa->action) ||
                (bcmFieldActionEgressPortsAdd    == pa->action) ||
                (bcmFieldActionRedirectMcast     == pa->action) ||
                (bcmFieldActionRedirectIpmc      == pa->action) ||
                (bcmFieldActionMirrorIngress     == pa->action) ||
                (bcmFieldActionMirrorEgress      == pa->action)) {

                if (_REGEX_ACTION_MODIFY & pa->flags) {

                    pa->flags &= ~_REGEX_ACTION_MODIFY;

                    pa_iter = p_ent->actions;

                    while (pa_iter != NULL) {
                        if ((_REGEX_ACTION_VALID == (pa_iter->flags & _REGEX_ACTION_VALID)) &&
                            (pa_iter->action == pa->action) &&
                            (_REGEX_INVALID_INDEX == pa_iter->hw_index)) {
                            /* Do not free here. Release the hardware
                             * index after policy install operation.
                             */
                            pa_iter->hw_index  = pa->hw_index;
                            pa_iter->flags    |= _REGEX_ACTION_MODIFY;
                            pa->hw_index       = _REGEX_INVALID_INDEX;
                            break;
                        }
                        pa_iter = pa_iter->next;
                    }

                    /* Here _REGEX_ACTION_MODIFY flag cleared means, no duplicate
                       actions are found i.e an action is removed from the entry.
                       Those invalid but not modified entries will be removed in
                       the second pass to this function,so just return BCM_E_NONE
                       on first pass */
                    if (!(_REGEX_ACTION_MODIFY & pa->flags)) {
                        return (BCM_E_NONE);
                    }
                }
            }

            /*
             * Action not set for this entry, free the hardware index.
             */
            if (_REGEX_INVALID_INDEX != pa->hw_index) {
                rv = _regex_policy_action_hw_resources_free(unit, p_ent, pa, _REGEX_ACTION_RESOURCE_FREE);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        if (pa_prev != NULL) {
            pa_prev->next = pa->next;
        } else { /* matched head of list */
            p_ent->actions = pa_next;
        }

        /* okay to free action */
        sal_free(pa);
        pa = pa_next;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_mtp_hw_free
 *
 * Purpose:
 *     Free mtp indexes used in regex policy
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy structure.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policy_mtp_hw_free(int unit, _regex_policy_t *p_ent, uint32 flags)
{
    _regex_policy_action_t *pa;
    int                     rv;         /* Operation return status. */

    /* Extract action info from the entry structure. */
    for (pa = p_ent->actions; pa != NULL; pa = pa->next) {
        /* Allocate mtp destination only for mirroring actions */
        if ((bcmFieldActionMirrorIngress != pa->action) &&
            (bcmFieldActionMirrorEgress != pa->action)) {
            continue;
        }

        if ((_REGEX_INVALID_INDEX == pa->hw_index) && (_REGEX_INVALID_INDEX == pa->old_index) ) {
            continue;
        }

        rv = _regex_policy_action_hw_resources_free(unit, p_ent, pa, flags);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_mtp_hw_alloc
 *
 * Purpose:
 *     Allocate mtp indexes required for policy installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy structure.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policy_mtp_hw_alloc(int unit, _regex_policy_t *p_ent)
{
    _regex_policy_action_t *pa;
    int                     rv;         /* Operation return status. */

    /* Extract action info from the entry structure. */
    for (pa = p_ent->actions; ((pa != NULL) && (_REGEX_ACTION_VALID & pa->flags)); pa = pa->next) {
        /* Allocate mtp destination only for mirroring actions */
        if ((bcmFieldActionMirrorIngress != pa->action) &&
            (bcmFieldActionMirrorEgress != pa->action)) {
            continue;
        }
        if (_REGEX_ACTION_MODIFY & pa->flags) {
            pa->old_index = pa->hw_index;
            pa->hw_index = _REGEX_INVALID_INDEX;
        }
        if (_REGEX_INVALID_INDEX == pa->hw_index) {
            /* Allocate mirror destination. */
            rv = _regex_policy_mirror_dest_add(unit, p_ent, pa);
            if (BCM_E_RESOURCE == rv) {
                if (_REGEX_ACTION_MODIFY & pa->flags) {
                    pa->hw_index = pa->old_index;
                    pa->old_index = _REGEX_INVALID_INDEX;
                }
            }
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_redirect_profile_hw_free
 *
 * Purpose:
 *     Free redirect profile indexes required for entry installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry descriptor.
 *     flags     - (IN) Free flags (old/new/both).
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policy_redirect_profile_hw_free(int unit, _regex_policy_t *p_ent, uint32 flags)
{
    _bcm_esw_regex_device_info_t *device;    /* Regex control structure. */
    _regex_policy_action_t       *pa;
    int                           rv = BCM_E_NONE; /* Operation return status. */

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Applicable to stage ingress on TRX devices only. */

    /* Extract the policy info from the entry structure. */
    for (pa = p_ent->actions; pa != NULL; pa = pa->next) {
        switch (pa->action) {
            case bcmFieldActionRedirectMcast:
#if defined(INCLUDE_L3)
            case bcmFieldActionRedirectIpmc:
#endif /* INCLUDE_L3 */
                if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)){
                    break;
                }
            case bcmFieldActionRedirectPbmp:
            case bcmFieldActionRedirectBcastPbmp:
            case bcmFieldActionEgressMask:
            case bcmFieldActionEgressPortsAdd:
                if ((flags & _REGEX_ACTION_RESOURCE_FREE) &&
                    (_REGEX_INVALID_INDEX != pa->hw_index)) {
                    rv = device->functions->redirect_profile_delete(unit,
                                                                    pa->hw_index);
                    BCM_IF_ERROR_RETURN(rv);
                    pa->hw_index = _REGEX_INVALID_INDEX;
                }

                if ((flags & _REGEX_ACTION_OLD_RESOURCE_FREE) &&
                    (_REGEX_INVALID_INDEX != pa->old_index)) {
                    rv = device->functions->redirect_profile_delete(unit,
                                                                    pa->old_index);
                    BCM_IF_ERROR_RETURN(rv);
                    pa->old_index = _REGEX_INVALID_INDEX;
                }
                break;
            default:
                break;
        }
    }
    return (rv);
}


/*
 * Function:
 *     _regex_policy_redirect_profile_hw_alloc
 *
 * Purpose:
 *     Allocate redirect profile index required for profile installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex profile structure.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policy_redirect_profile_hw_alloc(int unit, _regex_policy_t *p_ent)
{
    _bcm_esw_regex_device_info_t *device;    /* Regex control structure. */
    int                     rv = BCM_E_NONE; /* Operation return status.     */
    _regex_policy_action_t *pa;
    int                     ref_count;  /* Profile use reference count. */

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Applicable to stage ingress on TRX devices only. */

    /* Extract the policy info from the entry structure. */
    for (pa = p_ent->actions; ((pa != NULL) && (_REGEX_ACTION_VALID & pa->flags)); pa = pa->next) {
        switch (pa->action) {
            case bcmFieldActionRedirectMcast:
#if defined(INCLUDE_L3)
            case bcmFieldActionRedirectIpmc:
#endif /* INCLUDE_L3 */
                if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)){
                    break;
                }
            case bcmFieldActionRedirectPbmp:
            case bcmFieldActionRedirectBcastPbmp:
            case bcmFieldActionEgressMask:
            case bcmFieldActionEgressPortsAdd:
                /*
                 * Store previous hardware index value in old_index.
                 */
                pa->old_index = pa->hw_index;

                rv = device->functions->redirect_profile_alloc(unit, p_ent, pa);
                if ((BCM_E_RESOURCE == rv) &&
                    (_REGEX_INVALID_INDEX != pa->old_index)) {
                    /* Destroy old profile ONLY
                     * if it is not used by other entries.
                     */
                    rv = device->functions->redirect_profile_ref_count_get(unit,
                                                                           pa->old_index,
                                                                           &ref_count);
                    BCM_IF_ERROR_RETURN(rv);
                    if (ref_count > 1) {
                        return (BCM_E_RESOURCE);
                    }
                    rv = device->functions->redirect_profile_delete(unit, pa->old_index);
                    BCM_IF_ERROR_RETURN(rv);

                    /* Destroy old profile is no longer required. */
                    pa->old_index = _REGEX_INVALID_INDEX;

                    /* Reallocate profile for new action. */
                    rv = device->functions->redirect_profile_alloc(unit, p_ent, pa);
                }
                break;
            default:
                break;
        }
        if (BCM_FAILURE(rv)) {
            _regex_policy_redirect_profile_hw_free(unit, p_ent, _REGEX_ACTION_HW_FREE);
            break;
        }
    }

    return (rv);
}

STATIC int _regex_policers_compat(int               unit,
                                  _regex_policy_t  *p_ent,
                                  unsigned          lvl,
                                  _field_policer_t *f_pl)
{
#if defined(ESW_REGEX_HIERARCHICAL_METERS)
    const unsigned olvl        = 1 - lvl;
    _field_policer_t *_f_pl[2] = { 0, 0 };

    _f_pl[lvl] = f_pl;

    if (!(p_ent->policer[olvl].flags & _FP_POLICER_VALID)) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_bcm_regex_policer_get(unit,
                                               p_ent->policer[olvl].pid,
                                               &_f_pl[olvl]));

    switch (_f_pl[1]->cfg.flags & BCM_POLICER_COLOR_MERGE_MASK) {
        case BCM_POLICER_COLOR_MERGE_AND:
        case BCM_POLICER_COLOR_MERGE_OR:
            /* Level 1 sharing mode is SINGLE_AND or SINGLE_OR
               => level 0 and level 1 must both be in flow or modified
               trTCM mode
            */

            if ((IS_FLOW_MODE(_f_pl[0]) && IS_FLOW_MODE(_f_pl[1])) ||
                (IS_MOD_TRTCM_MODE(_f_pl[0]) && IS_MOD_TRTCM_MODE(_f_pl[1]))) {
                break;
            }
            return (BCM_E_PARAM);

        case BCM_POLICER_COLOR_MERGE_DUAL:
            /* Level 1 sharing mode is DUAL
               => level 0 must be modified trTCM
            */

            if (IS_MOD_TRTCM_MODE(_f_pl[0])) {
                break;
            }
            return (BCM_E_PARAM);

        default:
            return (BCM_E_PARAM);
    }
#endif
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _regex_policer_mode_support
 * Purpose:
 *      Validate policer mode support.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      p_ent   - (IN) Policy to which policer is attached.
 *      level   - (IN) Level policer attached.
 *      f_pl    - (IN) Policer descriptor.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int _regex_policer_mode_support(int               unit,
                                       _regex_policy_t  *p_ent,
                                       int               lvl,
                                       _field_policer_t *f_pl)
{
    switch (f_pl->cfg.mode) {
        case bcmPolicerModeGreen:
        case bcmPolicerModePassThrough:
        case bcmPolicerModeCommitted:
        case bcmPolicerModeTrTcmDs:
        case bcmPolicerModeCoupledTrTcmDs:
            break;
        case bcmPolicerModeSrTcm:
        case bcmPolicerModeSrTcmModified:
        case bcmPolicerModeTrTcm:
            if (lvl == 1) {
                return (BCM_E_PARAM);
            }
            break;
        default:
            return (BCM_E_PARAM);
    }

    return _regex_policers_compat(unit, p_ent, lvl, f_pl);
}

/*
 * Function:
 *      _regex_policer_hw_flags_set
 * Purpose:
 *      Update policer installation is required flag.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      f_pl    - (IN/OUT) Internal policer descriptor.
 *      flags   - (IN) Internal flags.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policer_hw_flags_set(int unit, _field_policer_t *f_pl, uint32 flags)
{
    /* Input parameters check. */
    if (NULL == f_pl) {
        return (BCM_E_PARAM);
    }

    f_pl->hw_flags |= flags;

    switch (f_pl->cfg.mode) {
        case bcmPolicerModeSrTcm:
        case bcmPolicerModeTrTcm:
        case bcmPolicerModeTrTcmDs:
        case bcmPolicerModeCoupledTrTcmDs:
        case bcmPolicerModeSrTcmModified:
        case bcmPolicerModeCommitted:
            f_pl->hw_flags |= _FP_POLICER_DIRTY;
            break;
        default:
            return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policer_hw_free
 *
 * Purpose:
 *     Deallocate hw policer from a policy.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     level     - (IN) Policer level.
 *     p_ent     - (IN) Policy that policer belongs to.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policer_hw_free(int unit, uint8 level, _regex_policy_t *p_ent)
{
    _field_entry_policer_t *f_ent_pl;  /* Field entry policer structure. */
    _field_policer_t       *f_pl;      /* Policer descriptor.            */

    /* Input parameters check. */
    if ((NULL == p_ent) || (level >= _REGEX_POLICER_LEVEL_COUNT)) {
        return (BCM_E_PARAM);
    }

    f_ent_pl = p_ent->policer + level;
    /* Skip uninstalled policers. */
    if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
        return (BCM_E_NONE);
    }

    /* Read policer configuration.*/
    BCM_IF_ERROR_RETURN(_bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl));

    /* Decrement hw reference count. */
    if (f_pl->hw_ref_count > 0) {
        f_pl->hw_ref_count--;
    }

    /* Policer not used by any other entry. */
    if (f_pl->hw_ref_count == 0) {
        /* Mark policer for reinstallation. */
        BCM_IF_ERROR_RETURN(_regex_policer_hw_flags_set(unit, f_pl, 0));
    }
    f_ent_pl->flags &= ~_FP_POLICER_INSTALLED;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policers_hw_alloc
 *
 * Purpose:
 *     Allocate policers required for policy installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry array.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     This API should be STATIC but is called by the legacy
 *     bregex code in tr3.
 */
int _regex_policers_hw_alloc(int unit, _regex_policy_t *p_ent)
{
    _bcm_esw_regex_device_info_t *device;   /* Regex control structure. */
    _field_entry_policer_t       *f_ent_pl; /* Field entry policer structure.*/
    _field_policer_t             *f_pl;     /* Field policer descriptor.     */
    int                           idx;      /* Policer levels iterator.      */
    int                           rv;       /* Operation return status.      */

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Allocate policers if necessary. */
    for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = p_ent->policer + idx;
        /* Skip invalid policers. */
        if (0 == (f_ent_pl->flags & _FP_POLICER_VALID)) {
            continue;
        }

        /* Read policer configuration.*/
        rv = _bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl);
        BCM_IF_ERROR_RETURN(rv);

        /*
         * Check policer mode support - double check in case
         * attached policer was modified by bcm_policer_set API.
         */
        rv = _regex_policer_mode_support(unit, p_ent, idx, f_pl);
        BCM_IF_ERROR_RETURN(rv);

        /* Calculate HW index */
        f_pl->hw_index   = _REGEX_POLICER_ID_TO_INDEX(f_pl->pid);
        f_pl->pool_index = device->meter_pool;

        /* Increment hw reference count. */
        if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
            f_ent_pl->flags |=  _FP_POLICER_INSTALLED;
            f_pl->hw_ref_count++;
        }

        if (f_pl->hw_flags & _FP_POLICER_DIRTY) {
            BCM_IF_ERROR_RETURN(device->functions->policer_install(unit, f_pl));
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policers_hw_free
 *
 * Purpose:
 *     Free entry policers.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy structure.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policers_hw_free(int unit, _regex_policy_t *p_ent)
{
    _field_entry_policer_t *f_ent_pl;  /* Field entry policer structure.*/
    int                    idx;        /* Policer levels iterator.      */

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Free policers if necessary. */
    for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = p_ent->policer + idx;
        /* Skip invalid policers. */
        if (0 == (f_ent_pl->flags & _FP_POLICER_VALID)) {
            continue;
        }
        /* Skip installed policers. */
        if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
            continue;
        }

        BCM_IF_ERROR_RETURN(_regex_policer_hw_free(unit, idx, p_ent));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_hw_resources_free
 *
 * Purpose:
 *     Free hw resources used in FP entry.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     p_ent     - (IN) Regex policy structure.
 *     flags     - (IN) Old/installed resource to free.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_hw_resources_free(int unit, _regex_policy_t *p_ent, uint32 flags)
{
    int rv;   /* Operation return status. */

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Free redirection profiles. */
    rv = _regex_policy_redirect_profile_hw_free(unit, p_ent, flags);
    BCM_IF_ERROR_RETURN(rv);

    /* Free mirror destination indexes. */
    rv = _regex_policy_mtp_hw_free(unit, p_ent, flags);
    BCM_IF_ERROR_RETURN(rv);

#ifdef ESW_REGEX_STAT_SUPPORT
    /* Free counters. */
    if (flags & _REGEX_ACTION_RESOURCE_FREE) {
        rv = _field_stat_hw_free(unit, p_ent);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif

    /* Free policers. */
    if (flags & _REGEX_ACTION_RESOURCE_FREE) {
        rv = _regex_policers_hw_free(unit, p_ent);
        BCM_IF_ERROR_RETURN(rv);
    }

#if defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_field_action_l2_change)) {
        rv = _regex_policy_l2_actions_hw_free(unit, p_ent, flags);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* INCLUDE_L3 */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_hw_resources_alloc
 *
 * Purpose:
 *     Allocate hw resources required for entry installation.
 *
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Entry array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_hw_resources_alloc(int unit, _regex_policy_t *p_ent)
{
    int rv;                        /* Operation return status. */

    /* Input parameters check. */
    if (NULL == p_ent) {
        return (BCM_E_PARAM);
    }

    /* Allocate policers. */
    BCM_IF_ERROR_RETURN(_regex_policers_hw_alloc(unit, p_ent));

#ifdef ESW_REGEX_STAT_SUPPORT
    /* Allocate statistics. */
    rv = _field_stat_hw_alloc(unit, p_ent);
    if (BCM_FAILURE(rv)) {
        _regex_policers_hw_free(unit, p_ent);
        return (rv);
    }
#endif

    /* Allocate mirror destination indexes. */
    /* COVERITY
     * Internally calls bcm_esw_mirror_enable()
     * routine, which has most stack use: 7844 bytes
     */
    /* coverity[stack_use_overflow : FALSE] */
    rv = _regex_policy_mtp_hw_alloc(unit, p_ent);
    if (BCM_FAILURE(rv)) {
#ifdef ESW_REGEX_STAT_SUPPORT
        _field_stat_hw_free(unit, p_ent);
#endif
        _regex_policers_hw_free(unit, p_ent);
        return (rv);
    }

    /* Allocate redirection profiles. */
    rv = _regex_policy_redirect_profile_hw_alloc(unit, p_ent);
    if (BCM_FAILURE(rv)) {
        _regex_policy_mtp_hw_free(unit, p_ent, _REGEX_ACTION_HW_FREE);
#ifdef ESW_REGEX_STAT_SUPPORT
        _field_stat_hw_free(unit, p_ent);
#endif
        _regex_policers_hw_free(unit, p_ent);
        return (rv);
    }

#if defined (INCLUDE_L3)
    if (soc_feature(unit, soc_feature_field_action_l2_change)) {
        rv =  _regex_policy_l2_actions_hw_alloc(unit, p_ent);
        if (BCM_FAILURE(rv)) {
            _regex_policy_mtp_hw_free(unit, p_ent, _REGEX_ACTION_HW_FREE);
#ifdef ESW_REGEX_STAT_SUPPORT
            _field_stat_hw_free(unit, p_ent);
#endif
            _regex_policers_hw_free(unit, p_ent);
            _regex_policy_redirect_profile_hw_free(unit, p_ent, _REGEX_ACTION_HW_FREE);
            return (rv);
        }
    }
#endif /* INCLUDE_L3 */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_install
 *
 * Purpose:
 *     Install a regex FT_POLICY entry. This writes the
 *     actions to the hardware.
 *
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     policy - (IN) Policy ID to be installed.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_install(int unit, bcm_regex_policy_t policy)
{
    _regex_policy_t              *p_ent; /* Policy entry pointer. */
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;   /* Operation return status.  */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s(%d)\n"),
               unit, FUNCTION_NAME(), policy));

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Installing the entry. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* If entry was previously installed, we better disable it temporarily,
     * to avoid intermittent actions.
     */
    if (p_ent->flags & _REGEX_POLICY_INSTALLED) {
        /* Disable installed entry. */
        rv = device->functions->policy_remove(unit, p_ent);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Remove deleted actions. */
    rv = _regex_policy_invalid_actions_remove(unit, p_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* Allocate hw resources required for policy installation. */
    rv = _regex_policy_hw_resources_alloc(unit, p_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* Install physical policy */
    rv = device->functions->policy_install(unit, p_ent);
    if (BCM_FAILURE(rv)) {
        (void)_regex_policy_hw_resources_free(unit, p_ent, _REGEX_ACTION_HW_FREE);
        return (rv);
    }
    p_ent->flags &= ~_REGEX_POLICY_DIRTY;
    p_ent->flags |= _REGEX_POLICY_INSTALLED;

    /* In case of policy reinstall free previous installation hw resources. */
    rv = _regex_policy_hw_resources_free(unit, p_ent, _REGEX_ACTION_OLD_RESOURCE_FREE);
    if (BCM_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN(_regex_policy_hw_resources_free(unit, p_ent, _REGEX_ACTION_HW_FREE));
        return (rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_remove
 *
 * Purpose:
 *     Remove a policy from the hardware tables.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     policy - (IN) policy to remove.
 *
 * Returns:
 *     BCM_E_XXX
 */
int _regex_policy_remove(int unit, bcm_regex_policy_t policy)
{
    _regex_policy_t              *p_ent; /* Policy entry pointer. */
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;   /* Operation return status.  */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s(%d)\n"),
               unit, FUNCTION_NAME(), policy));

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (!(p_ent->flags & _REGEX_POLICY_INSTALLED)) {
        /* Entry not installed in hardware to perform remove operation. */
        return (BCM_E_NONE);
    }

    /* Free hw resources allocated during entry installation. */
    rv = _regex_policy_hw_resources_free(unit, p_ent, _REGEX_ACTION_RESOURCE_FREE);
    BCM_IF_ERROR_RETURN(rv);

    rv = device->functions->policy_remove(unit, p_ent);
    BCM_IF_ERROR_RETURN(rv);

    p_ent->flags |= _REGEX_POLICY_DIRTY;
    p_ent->flags &= ~_REGEX_POLICY_INSTALLED;


    /* Remove deleted actions. */
    rv = _regex_policy_invalid_actions_remove(unit, p_ent);
    BCM_IF_ERROR_RETURN(rv);
    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_create_id
 *
 * Purpose:
 *     Create a blank entry group based on a group;
 *     allows selection of a specific slot in a slice
 *
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     policy - (IN) Policy id.
 * Returns:
 *     BCM_E_INIT      - unit not initialized
 *     BCM_E_EXISTS    - Policy ID already in use
 *     BCM_E_MEMORY    - allocation failure
 *     BCM_E_NONE      - Success
 */
STATIC int _regex_policy_create_id(int unit, bcm_regex_policy_t policy)
{
    _regex_policy_t              *p_ent;  /* Allocated entry buffer.      */
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;     /* Operation return status.     */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s(policy=%d)\n"),
               unit, FUNCTION_NAME(), policy));

    /* Confirm that 'policy' is not already used on unit */
    if (BCM_SUCCESS(_regex_policy_get(unit, policy, &p_ent))) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: policy=(%d) already exists.\n"),
                   unit, policy));
        return (BCM_E_EXISTS);
    }

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (device->num_policies >= ESW_REGEX_POLICIES_MAX(unit)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: No room.\n"),
                   unit));
        return (BCM_E_RESOURCE);
    }

    /* Create entry structure. */
    rv = _regex_policy_phys_create(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _regex_policy_action_alloc
 * Purpose:
 *     Allocate and initialize an action structure.
 * Parameters:
 *     unit     - BCM device number.
 *     action   - Action to perform (bcmFieldActionXXX)
 *     param0   - Action parameter (use 0 if not required)
 *     param1   - Action parameter (use 0 if not required)
 *     param2   - Action parameter (use 0 if not required)
 *     param3   - Action parameter (use 0 if not required)
 *     fa (OUT) - pointer to field action structure
 * Returns:
 *     BCM_E_MEMORY - allocation failure
 *     BCM_E_NONE   - Success
 */
int _regex_policy_action_alloc(int unit, bcm_field_action_t action,
                               uint32 param0, uint32 param1, uint32 param2,
                               uint32 param3, _regex_policy_action_t **pa)
{
    *pa = sal_alloc(sizeof(**pa), "regex_policy_action");
    if (NULL == *pa) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: allocation failure for regex_policy_action\n"),
                   unit));
        return (BCM_E_MEMORY);
    }
    sal_memset(*pa, 0, sizeof(**pa));

    (*pa)->action = action;
    (*pa)->param[0] = param0;
    (*pa)->param[1] = param1;
    (*pa)->param[2] = param2;
    (*pa)->param[3] = param3;
    (*pa)->hw_index = _REGEX_INVALID_INDEX;
    (*pa)->old_index = _REGEX_INVALID_INDEX;
    /* mark new action as not yet installed */
    (*pa)->flags |= (_REGEX_ACTION_VALID | _REGEX_ACTION_DIRTY);

    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_action_add
 *
 * Purpose:
 *     Add action performed when policy is applied to a session.
 *
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     policy - (IN) Policy entry id.
 *     pa     - (IN) Policy action structure.
 * Returns:
 *     BCM_E_XXX
 */
int _regex_policy_action_add(int unit, bcm_regex_policy_t policy, _regex_policy_action_t *pa)
{
    _regex_policy_t              *p_ent; /* Field entry structure.       */
    _regex_policy_action_t       *pa_iter; /* Field entry actions iterator.*/
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;   /* Operation return status.     */

    /* Get policy structure pointer. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    BCM_IF_ERROR_RETURN(rv);

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Ensure action is supported. */
    rv = device->functions->policy_action_support_check(unit,
                                                        pa->action);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: action=%s is not supported.\n"),
                   unit,
                   _regex_policy_action_name(pa->action)));
        return (rv);
    }

    /* Check for existing actions that conflict with the new action. */
    pa_iter = p_ent->actions;
    while (pa_iter != NULL) {
        if (0 == (pa_iter->flags & _REGEX_ACTION_VALID)) {
            pa_iter = pa_iter->next;
            continue;
        }
        rv = device->functions->policy_action_conflict_check(unit,
                                                            pa_iter->action,
                                                            pa->action);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_REGEX,
                      (BSL_META_U(unit,
                                  "FT(unit %d) Error: action=%s conflicts with existing action in policy=%d\n"),
                       unit,
                       _regex_policy_action_name(pa->action),
                       policy));
            return (rv);
        }
        pa_iter = pa_iter->next;
    }

    /* Check action parameters. */
    rv = device->functions->policy_action_params_check(unit, p_ent, pa);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: action=%s parameters check failed (%d)\n"),
                   unit, _regex_policy_action_name(pa->action), rv));
        return (rv);
    }

    if (pa->action == bcmFieldActionColorIndependent) {
        if (0 != pa->param[0])  {
            p_ent->flags |= _REGEX_POLICY_COLOR_INDEPENDENT;
        } else {
            p_ent->flags &= ~_REGEX_POLICY_COLOR_INDEPENDENT;
        }
        p_ent->flags  |= _REGEX_POLICY_DIRTY;
        sal_free(pa);
        return (BCM_E_NONE);
    }

    /* Add action to front of entry's linked-list. */
    ESW_REGEX_LOCK(unit);
    pa->next = p_ent->actions;
    p_ent->actions  = pa;
    p_ent->flags    |= _REGEX_POLICY_DIRTY;
    ESW_REGEX_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_action_ports_add
 *
 * Purpose:
 *     Add action performed when policy is applied to a session.
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     policy - (IN) Policy entry id.
 *     action - (IN) Action to perform (bcmFieldActionXXX)
 *     param0 - (IN) Action parameter (use 0 if not required)
 *     param1 - (IN) Action parameter (use 0 if not required)
 *     param2 - (IN) Action parameter (use 0 if not required)
 *     param3 - (IN) Action parameter (use 0 if not required)
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int _regex_policy_action_ports_add(int unit,
                                          bcm_regex_policy_t policy,
                                          bcm_field_action_t action,
                                          uint32 param0,
                                          uint32 param1,
                                          uint32 param2,
                                          uint32 param3)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_action_t       *pa = NULL; /* Policy action descriptor. */
    int                           rv;   /* Operation return status. */

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s(policy=%d, action=%s, p0=%d, p1=%d, p2=%d)\n"),
               unit, FUNCTION_NAME(), policy,
               _regex_policy_action_name(action),
               param0, param1, param2));

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }
    ESW_REGEX_LOCK(unit);

    /*
     * Allocate the action  descriptor.
     */
    rv = _regex_policy_action_alloc(unit, action, param0, param1, param2, param3, &pa);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT Error: _regex_policy_action_alloc failed\n")));
        ESW_REGEX_UNLOCK(unit);
        return rv;
    }

    /*
     * Add action to entry actions list.
     */
    rv = _regex_policy_action_add(unit, policy, pa);
    ESW_REGEX_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        sal_free(pa);
        return rv;
    }
    return (BCM_E_NONE);
}

/*
 * Function: _regex_policy_action_ports_get
 *
 * Purpose:
 *     Get parameters associated with a policy action
 *
 * Parameters:
 *     unit   - (IN) BCM device number
 *     policy - (IN) Policy entry id.
 *     action - (IN) Action to perform (bcmFieldActionXXX)
 *     param0 - (OUT) Action parameter
 *     param1 - (OUT) Action parameter
 *     param2 - (OUT) Action parameter
 *     param3 - (OUT) Action parameter
 *
 * Returns:
 *     BCM_E_INIT      - BCM Unit not initialized
 *     BCM_E_NOT_FOUND - Entry ID not found
 *     BCM_E_NOT_FOUND - No matching Action for entry
 *     BCM_E_PARAM     - paramX is NULL
 *     BCM_E_NONE      - Success
 */

STATIC int _regex_policy_action_ports_get(int unit,
                                          bcm_regex_policy_t policy,
                                          bcm_field_action_t action,
                                          uint32 *param0,
                                          uint32 *param1,
                                          uint32 *param2,
                                          uint32 *param3)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_action_t       *pa = NULL; /* Policy action descriptor. */
    _regex_policy_t              *p_ent; /* Field entry structure.       */
    int                           rv;

    /* Input parameters check. */
    if ((NULL == param0) || (NULL == param1) || (NULL == param2)) {
            return (BCM_E_PARAM);
    }

    /* Lock the module. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }
    ESW_REGEX_LOCK(unit);

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* Find matching action in the entry */
    for (pa = p_ent->actions; pa != NULL; pa = pa->next) {
        if (pa->action == action) {
            break;
        }
    }

    if (pa == NULL) {
        ESW_REGEX_UNLOCK(unit);
        LOG_VERBOSE(BSL_LS_BCM_REGEX,
                    (BSL_META_U(unit,
                                "FT(unit %d) Error: action not in policy=%d\n"),
                     unit, policy));
        return (BCM_E_NOT_FOUND);
    }

    *param0 = pa->param[0];
    *param1 = pa->param[1];
    *param2 = pa->param[2];
    *param3 = pa->param[3];

    /* Unlock the module. */
    ESW_REGEX_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      _regex_policy_action_delete
 * Purpose:
 *      Delete action from a policy.
 * Parameters:
 *      unit   - (IN) BCM device number
 *      policy - (IN) Policy entry id.
 *      action - (IN) Action to remove (bcmFieldActionXXX)
 *      param0 - (IN) Action parameter (use 0 if not required)
 *      param1 - (IN) Action parameter (use 0 if not required)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _regex_policy_action_delete(int unit, bcm_regex_policy_t policy,
                                bcm_field_action_t action,
                                uint32 param0, uint32 param1)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_action_t       *pa      = NULL; /* Policy action descriptor. */
    _regex_policy_action_t       *pa_prev = NULL;
    _regex_policy_t              *p_ent; /* Field entry structure.       */
    int                           rv;   /* Operation return status. */

    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Get field entry descriptor. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Special treatment for pre-historic actions. */
    switch (action) {
        case  bcmFieldActionColorIndependent:
            p_ent->flags |= _REGEX_POLICY_DIRTY;
            p_ent->flags &= ~_REGEX_POLICY_COLOR_INDEPENDENT;
            return (BCM_E_NONE);
        default:
            break;
    }

    ESW_REGEX_LOCK(unit);

    /* Find the action in the entry */
    pa = p_ent->actions; /* start at head */

    while (pa != NULL) {
        if (pa->action != action) {
            pa_prev = pa;
            pa      = pa->next;
            continue;
        }

        /* Some actions might be applied multiple times.*/
        if (((action == bcmFieldActionMirrorIngress) ||
             (action == bcmFieldActionMirrorEgress)) &&
            (((uint32)_REGEX_INVALID_INDEX != param0) &&
             ((uint32)_REGEX_INVALID_INDEX != param1)))  {
            if ((pa->param[0] != param0)  || (pa->param[1] != param1)) {
                pa_prev = pa;
                pa      = pa->next;
                continue;
            }
        }
        break;
    }

    if (NULL == pa) {
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_NOT_FOUND);
    }

    /* If entry is installed mark action as invalid.
     * Invalid actions will be removed during entry remove/reinstall.
     */
    if (p_ent->flags & _REGEX_POLICY_INSTALLED) {
        pa->flags  &= ~_REGEX_ACTION_VALID;
        pa->flags |= _REGEX_ACTION_MODIFY;
        p_ent->flags |= _REGEX_POLICY_DIRTY;
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_NONE);
    }

    if (pa_prev != NULL) {
        pa_prev->next = pa->next;
    } else { /* matched head of list */
        p_ent->actions = pa->next;
    }

    /* okay to free action */
    sal_free(pa);
    pa = NULL;

    p_ent->flags |= _REGEX_POLICY_DIRTY;
    ESW_REGEX_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_create
 * Purpose:
 *      Create a blank regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      flags  - (IN) REGEX_FLOW_xxx flags
 *      policy - (OUT) ID of policy to be created.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_create(int unit, int flags, bcm_regex_policy_t *policy)
{
    _bcm_esw_regex_device_info_t  *device; /* Regex control structure. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    if (0 == (flags & BCM_REGEX_POLICY_WITH_ID))
    {
        /* Generate a policy ID. */
        do {
            last_allocated_policy++;
            if (ESW_REGEX_POLICIES_LAST(unit) < last_allocated_policy) {
                last_allocated_policy = ESW_REGEX_POLICIES_LAST(unit);
            }
        } while (BCM_SUCCESS(_regex_policy_get(unit, last_allocated_policy, NULL)));

        *policy = last_allocated_policy;
    }

    return _regex_policy_create_id(unit, *policy);
}

/*
 * Function:
 *      bcm_esw_regex_policy_destroy
 * Purpose:
 *      Destroy a regex policy. This deallocates memory only.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be destroyed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_destroy(int unit, bcm_regex_policy_t policy)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_t              *p_ent; /* Allocated entry buffer.      */
    int                           rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Cannot delete policy 0 since it is used by the flow tracker for new and unmatched
     * sessions. */
    if (_REGEX_DEFAULT_POLICY_ID == policy) {
        return BCM_E_BUSY;
    }

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    ESW_REGEX_LOCK(unit);

    /* Remove policy from hw before destroying sw image. */
    if (p_ent->flags & _REGEX_POLICY_INSTALLED) {
        rv = bcm_esw_regex_policy_remove(unit, policy);
        if (BCM_FAILURE(rv)) {
            ESW_REGEX_UNLOCK(unit);
            return(rv);
        }
    }

    /* Destroy policy policers */
    rv = bcm_esw_regex_policy_policer_detach_all(unit, policy);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return(rv);
    }

#ifdef ESW_REGEX_STAT_SUPPORT
    /* Destroy policy stats */
    if (p_ent->statistic.flags & _REGEX_POLICY_STAT_VALID) {
        rv = bcm_esw_regex_policy_stat_detach(unit, policy, p_ent->statistic.sid);
        if (BCM_FAILURE(rv)) {
            ESW_REGEX_UNLOCK(unit);
            return(rv);
        }
    }
#endif

    /* Destroy entry sw images. */
    rv = _regex_policy_phys_destroy(unit, p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return(rv);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    ESW_REGEX_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_destroy_all
 * Purpose:
 *      Destroy all regex policy entries.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_destroy_all(int unit)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           idx;
    int                           rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s()\n"),
               unit, FUNCTION_NAME()));

    ESW_REGEX_LOCK(unit);

    /* Scan unit's groups freeing each entry list. */
    for (idx = device->num_policies - 1; idx >= 0; idx--) {
        /* Destroy all policies except the default policy. */
        if (_REGEX_DEFAULT_POLICY_ID == device->policies[idx]->eid) {
            continue;
        }
        rv =  bcm_esw_regex_policy_destroy(unit, (device->policies[idx])->eid);
        if (BCM_FAILURE(rv)) {
            ESW_REGEX_UNLOCK(unit);
            return (rv);
        }
    }

    ESW_REGEX_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_install
 * Purpose:
 *      Install a regex policy into the hardware tables.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be installed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_install(int unit, bcm_regex_policy_t policy)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_t              *p_ent; /* Allocated entry buffer.      */
    int                           rv;
    int                           dirty; /* Field entry dirty flag.  */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s()\n"),
               unit, FUNCTION_NAME()));

    ESW_REGEX_LOCK(unit);

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return rv;
    }

    rv = _regex_policy_dirty(unit, p_ent, &dirty);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* Write the Policy entry */
    if (dirty) {
        rv = _regex_policy_install(unit, policy);
    } else {
        rv = BCM_E_NONE;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    ESW_REGEX_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_regex_policy_remove
 * Purpose:
 *      Remove a regex policy from the hardware tables.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be removed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_remove(int unit, bcm_regex_policy_t policy)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                 rv;          /* Operation return status. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s()\n"),
               unit, FUNCTION_NAME()));

    ESW_REGEX_LOCK(unit);

    rv =  _regex_policy_remove(unit, policy);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    ESW_REGEX_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_add
 * Purpose:
 *      Add an action to a regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be modified.
 *      action - (IN) Field processor action value
 *      param0 - (IN) Action-dependent value; 0 if not used
 *      param1 - (IN) Action-dependent value; 0 if not used
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_add(int unit, bcm_regex_policy_t policy,
                                    bcm_field_action_t action, uint32 param0, uint32 param1)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_action_t       *pa = NULL; /* Policy action descriptor. */
    int                           rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_REGEX,
              (BSL_META_U(unit,
                          "FT(unit %d) vverb: %s(policy=%d, action=%s, p0=%d, p1=%d)\n"),
               unit, FUNCTION_NAME(), policy,
               _regex_policy_action_name(action), param0, param1));

    if ((SOC_PORT_ADDR_MAX(unit) > 31) &&
        ((action == bcmFieldActionRedirectPbmp)   ||
         (action == bcmFieldActionEgressMask)     ||
         (action == bcmFieldActionEgressPortsAdd) ||
         (action == bcmFieldActionRedirectBcastPbmp))) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Use bcm_regex_policy_action_ports_add api.\n"),
                   unit));
        return (BCM_E_PARAM);
    }

    /* Adapt action parameters */
    rv = _regex_policy_params_api_to_hw_adapt(unit, action, &param0, &param1);
    BCM_IF_ERROR_RETURN(rv);

    ESW_REGEX_LOCK(unit);

    /*
     * Allocate the action descriptor, param2 and param3 are not valid here.
     */
    rv = _regex_policy_action_alloc(unit, action, param0, param1, 0, 0, &pa);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: failure in _regex_policy_action_alloc()\n"),
                   unit));
        ESW_REGEX_UNLOCK(unit);
        return rv;
    }

    /*
     * Add action to entry actions list.
     */
    rv = _regex_policy_action_add(unit, policy, pa);
    ESW_REGEX_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        sal_free(pa);
        return rv;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_mac_add
 * Purpose:
 *      Add an action to a policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) Regex policy Id.
 *      action - (IN) Field action value
 *      mac    - (IN) MAC address for actions involving src/dst MAC.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_mac_add(int unit,
                                        bcm_regex_policy_t policy,
                                        bcm_field_action_t action,
                                        bcm_mac_t mac)
{
    uint32 param[_FT_ACTION_PARAM_SZ];

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if ((action != bcmFieldActionSrcMacNew) &&
        (action != bcmFieldActionDstMacNew)) {
        return (BCM_E_PARAM);
    }

    SAL_MAC_ADDR_TO_UINT32(mac, param);

    return bcm_esw_regex_policy_action_add(unit, policy, action,
                                           param[0],param[1]);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_ports_add
 * Purpose:
 *      Add an action to a regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) regex policy ID
 *      action - (IN) Field action value
 *      pbmp   - (IN) pbmp for actions involving port bitmap
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_ports_add(int unit,
                                          bcm_regex_policy_t policy,
                                          bcm_field_action_t action,
                                          bcm_pbmp_t pbmp)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    uint32 param[_FT_ACTION_PARAM_SZ];

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    if ((action != bcmFieldActionRedirectPbmp) &&
        (action != bcmFieldActionEgressMask) &&
        (action != bcmFieldActionEgressPortsAdd) &&
        (action != bcmFieldActionRedirectBcastPbmp)) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: Incorrect action parameter\n"),
                   unit));
        return (BCM_E_PARAM);
    }

    sal_memset(&param, 0, sizeof(param));

    param[0] = SOC_PBMP_WORD_GET(pbmp, 0);
    if (soc_feature(unit, soc_feature_table_hi)) {
        param[1] = SOC_PBMP_WORD_GET(pbmp, 1);
        /* Trident device can support more than 64-ports */
        if (SOC_IS_TD_TT(unit)) {
            param[2] = SOC_PBMP_WORD_GET(pbmp, 2);
        }
        /* Trident2 device can support more than 96-ports */
        if (SOC_IS_TD2_TT2(unit)) {
            param[3] = SOC_PBMP_WORD_GET(pbmp, 3);
            param[3] &= 0x3ff; /* TD2 has 106 ports */
        }
    }

    return _regex_policy_action_ports_add(unit, policy, action,
                                          param[0], param[1],
                                          param[2], param[3]);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_delete
 * Purpose:
 *      Delete an action from a regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be modified.
 *      action - (IN) Field processor action value
 *      param0 - (IN) Action-dependent value; 0 if not used
 *      param1 - (IN) Action-dependent value; 0 if not used
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_delete(int unit,
                                       bcm_regex_policy_t policy,
                                       bcm_field_action_t action,
                                       uint32 param0,
                                       uint32 param1)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;   /* Operation return status. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (action < 0 || bcmFieldActionCount <= action) {
        LOG_ERROR(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: unknown action=%d\n"),
                   unit, action));
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Adapt action parameters */
    rv = _regex_policy_params_api_to_hw_adapt(unit, action, &param0, &param1);
    BCM_IF_ERROR_RETURN(rv);

    ESW_REGEX_LOCK(unit);

    rv = _regex_policy_action_delete(unit, policy, action, param0, param1);

    ESW_REGEX_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_get
 * Purpose:
 *      Retrieve the parameters for an action previously added to a
 *      regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) regex policy ID.
 *      action - (IN) Field processor action value
 *      param0 - (OUT) Action-dependent value
 *      param1 - (OUT) Action-dependent value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_get(int unit,
                                    bcm_regex_policy_t policy,
                                    bcm_field_action_t action,
                                    uint32 *param0,
                                    uint32 *param1)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_t              *p_ent; /* Allocated entry buffer.      */
    _regex_policy_action_t       *pa = NULL; /* Policy action descriptor. */
    int                           rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check. */
    if ((NULL == param0) || (NULL == param1)) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    if (bcmFieldActionColorIndependent == action) {
        *param0 = (p_ent->flags & _REGEX_POLICY_COLOR_INDEPENDENT) ? 1 : 0;
        *param1 = 0;
        /* Unlock the module. */
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_NONE);
    }

    /* Find matching action in the entry */
    for (pa = p_ent->actions; pa != NULL; pa = pa->next) {
        if (pa->action == action) {
            break;
        }
    }

    if (pa == NULL) {
        ESW_REGEX_UNLOCK(unit);
        LOG_DEBUG(BSL_LS_BCM_REGEX,
                  (BSL_META_U(unit,
                              "FT(unit %d) Error: action not in policy=%d\n"),
                   unit, policy));
        return (BCM_E_NOT_FOUND);
    }

    *param0 = pa->param[0];
    *param1 = pa->param[1];

    /* Unlock the module. */
    ESW_REGEX_UNLOCK(unit);

    /* Adapt action parameters */
    rv = _regex_policy_params_hw_to_api_adapt(unit, action, param0, param1);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_mac_get
 * Purpose:
 *      Retrieve the parameters for an action previously added to a
 *      regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) Regex policy ID.
 *      action - (IN) Field action value.
 *      mac    - (OUT) MAC address for actions involving src/dst MAC.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_mac_get(int unit,
                                        bcm_regex_policy_t policy,
                                        bcm_field_action_t action,
                                        bcm_mac_t *mac)
{
    uint32 param[_FT_ACTION_PARAM_SZ];
    int    rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameter check. */
    if (NULL == mac) {
        return (BCM_E_PARAM);
    }

    if ((action != bcmFieldActionSrcMacNew) &&
        (action != bcmFieldActionDstMacNew)) {
        return (BCM_E_PARAM);
    }

    /* Extract action information from field entry. */
    rv = bcm_esw_regex_policy_action_get(unit, policy, action,
                                         param, param + 1);
    BCM_IF_ERROR_RETURN(rv);

    SAL_MAC_ADDR_FROM_UINT32((*mac), param);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_ports_get
 * Purpose:
 *      Retrieve the parameters for an action previously added to a
 *      regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) Regex policy ID
 *      action - (IN) Field action value
 *      pbmp   - (OUT) pbmp for actions involving port bitmap
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_ports_get(int unit,
                                          bcm_regex_policy_t policy,
                                          bcm_field_action_t action,
                                          bcm_pbmp_t *pbmp)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    uint32 param[_FT_ACTION_PARAM_SZ];
    int    rv;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Input parameter check. */
    if (NULL == pbmp) {
        return (BCM_E_PARAM);
    }

    if ((action != bcmFieldActionRedirectPbmp) &&
        (action != bcmFieldActionEgressMask) &&
        (action != bcmFieldActionRedirectBcastPbmp) &&
        (action != bcmFieldActionEgressPortsAdd)) {
        return (BCM_E_PARAM);
    }

    sal_memset(param, 0, sizeof(param));

    /* Extract action information from policy entry. */
    rv = _regex_policy_action_ports_get(unit, policy, action,
                                        param, param + 1, param + 2, param + 3);
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_CLEAR(*pbmp);
    SOC_PBMP_WORD_SET(*pbmp, 0, param[0]);

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_table_hi)) {
        SOC_PBMP_WORD_SET(*pbmp, 1, param[1]);
        if (SOC_IS_TD_TT(unit)) {
            SOC_PBMP_WORD_SET(*pbmp, 2, param[2]);
        }
        if (SOC_IS_TD2_TT2(unit)) {
            SOC_PBMP_WORD_SET(*pbmp, 3, param[3]);
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_remove
 * Purpose:
 *      Remove an action from a regex_policy.  Same as
 *      bcm_esw_regex_policy_action_delete for actions without parameters.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be modified.
 *      action - (IN) Field processor action value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_remove(int unit,
                                       bcm_regex_policy_t policy,
                                       bcm_field_action_t action)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    int                           rv;   /* Operation return status. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    rv = bcm_esw_regex_policy_action_delete(unit, policy, action,
                                            (uint32)_REGEX_INVALID_INDEX,
                                            (uint32)_REGEX_INVALID_INDEX);
    ESW_REGEX_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_policy_action_remove_all
 * Purpose:
 *      Remove all actions from a regex_policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) ID of policy to be modified.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_action_remove_all(int unit,
                                           bcm_regex_policy_t policy)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _regex_policy_t              *p_ent; /* Allocated entry buffer.      */
    _regex_policy_action_t       *pa = NULL; /* Policy action descriptor. */
    int                           rv;   /* Operation return status.  */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* start at the head of the actions list and burn them up */
    pa = p_ent->actions;

    while (pa != NULL) {
        rv = _regex_policy_action_delete(unit, policy, pa->action,
                                         pa->param[0], pa->param[1]);
        if (BCM_FAILURE(rv)) {
            ESW_REGEX_UNLOCK(unit);
            return (rv);
        }

        /*
         * Action might be removed from the entry actions
         * list only if entry was not installed.
         * Otherwise clean up will happen during reinstall.
         */
        pa = (p_ent->flags & _REGEX_POLICY_INSTALLED) ? pa->next : p_ent->actions;
    }

    p_ent->flags |= _REGEX_POLICY_DIRTY;

    ESW_REGEX_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *     _bcm_regex_policer_get
 * Purpose:
 *     Lookup a Policer description structure by policer id.
 * Parmeters:
 *     unit      - (IN)BCM device number.
 *     pid       - (IN)Policer id.
 *     policer_p - (OUT) Policer lookup result.
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_regex_policer_get(int unit, bcm_policer_t pid,
                           _field_policer_t **policer_p)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _field_policer_t             *f_pl;   /* Policer lookup pointer.  */

    /* Input parameters check. */
    if (NULL == policer_p) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    f_pl  =  device->policers[_REGEX_POLICER_ID_TO_INDEX(pid)];
    if (NULL != f_pl) {
        *policer_p = f_pl;
        return (BCM_E_NONE);
    }

    /* Policer with pid == pid was not found. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_esw_regex_policer_destroy
 * Purpose:
 *      Destroy a policer entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _regex_policer_destroy(int unit, bcm_policer_t pid)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure.     */
    _field_policer_t             *f_pl;   /* Internal policer descriptor. */
    int                           rv;     /* Operation return status.     */

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Find policer info. */
    rv = _bcm_regex_policer_get(unit, pid, &f_pl);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Reject destroy if policer is in use. */
    if (f_pl->sw_ref_count > 1) {
        return (BCM_E_BUSY);
    }

    /* Remove policer from policers table. */
    device->policers[_REGEX_POLICER_ID_TO_INDEX(pid)] = NULL;

    /* De-allocate policer descriptor. */
    sal_free(f_pl);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _regex_policer_id_alloc
 * Purpose:
 *      Allocate a policer id.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      pid     - (OUT) Policer id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _regex_policer_id_alloc(int unit, bcm_policer_t *pid)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure.     */
    int                           i;

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }
    /* Input parameters check. */
    if (NULL == pid) {
        return (BCM_E_PARAM);
    }

    for (i = 0; i < _REGEX_POLICER_MAX_NUM; i++) {
        if (NULL == device->policers[i]) {
            *pid = _REGEX_POLICER_ID_ENCODE(unit, i);
            return BCM_E_NONE;
        }
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *      _bcm_esw_regex_policer_create
 * Purpose:
 *      Initialize a policer entry.
 * Parameters:
 *      unit    - (IN) BCM device number.
 *      pol_cfg - (IN) Policer configuration.
 *      pid     - (OUT) Policer id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_regex_policer_create(int unit, bcm_policer_config_t *pol_cfg,
                                  bcm_policer_t *pid)
{
    _bcm_esw_regex_device_info_t *device;      /* Regex control structure.     */
    _field_policer_t             *f_pl = NULL; /* Internal policer descriptor. */
    int                           rv;          /* Operation return status.     */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == pol_cfg) {
        return (BCM_E_PARAM);
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Generate policer id. */
    if (0 == (pol_cfg->flags & BCM_POLICER_WITH_ID)) {
        rv = _regex_policer_id_alloc(unit, pid);
        if (BCM_FAILURE(rv)) {
            ESW_REGEX_UNLOCK(unit);
            return rv;
        }
    } else {
        /* Check if policer id is already in use. */
        rv = _bcm_regex_policer_get(unit, *pid, &f_pl);
        if (BCM_SUCCESS(rv)) {

            /* Verify that replace flag is set. */
            if (0 == (pol_cfg->flags & BCM_POLICER_REPLACE)) {
                ESW_REGEX_UNLOCK(unit);
                return (BCM_E_EXISTS);
            }

            /* Make sure policer is not attached to any entry. */
            if (1 != f_pl->sw_ref_count) {
                ESW_REGEX_UNLOCK(unit);
                return (BCM_E_BUSY);
            }
            /* Copy new configuration. */
            sal_memcpy(&f_pl->cfg, pol_cfg, sizeof(f_pl->cfg));

            /* Set policer "dirty" flag and return. */
            rv = _regex_policer_hw_flags_set(unit, f_pl, 0);
            ESW_REGEX_UNLOCK(unit);
            return (rv);
        }
    }

    /* Allocate policer descriptor. */
    f_pl = sal_alloc(sizeof(*f_pl), "Regex policer");
    if (NULL == f_pl) {
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_MEMORY);
    }

    /* Copy policer configuration. */
    sal_memcpy(&f_pl->cfg, pol_cfg, sizeof(f_pl->cfg));

    /* Set policer "dirty" flags. */
    rv = _regex_policer_hw_flags_set(unit, f_pl, 0);
    if (BCM_FAILURE(rv)) {
        sal_free(f_pl);
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* Initialize reference count to 1. */
    f_pl->sw_ref_count = 1;

    /* Set hw index to - no hw resources allocated. */
    f_pl->hw_index = _FP_INVALID_INDEX;

    /* Initialize policer id. */
    f_pl->pid = *pid;

    /* Insert policer into policers table. */
    device->policers[_REGEX_POLICER_ID_TO_INDEX(*pid)] = f_pl;

    ESW_REGEX_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_regex_policer_destroy
 * Purpose:
 *      Destroy a policer entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_regex_policer_destroy(int unit, bcm_policer_t pid)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure.     */
    int                           rv = BCM_E_NONE; /* Operation return status. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Destroy policer. */
    rv = _regex_policer_destroy(unit, pid);

    ESW_REGEX_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_policer_destroy_all
 * Purpose:
 *      Destroy all policer entries.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_esw_regex_policer_destroy_all(int unit)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure.     */
    int                           rv = BCM_E_NONE; /* Operation return status. */
    int                           idx;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Iterate over all entries. */
    for (idx = 0; idx < sizeof(device->policers)/sizeof(device->policers[0]); idx++) {
        if (NULL != device->policers[idx]) {
            rv = _regex_policer_destroy(unit, device->policers[idx]->pid);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    ESW_REGEX_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_policer_traverse
 * Purpose:
 *      Traverse all existing policer entries and call supplied
 *      callback function.
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) Callback function.
 *      user_data - (IN) User data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     To speed up traverse operation - no hw read is perfomed.
 */
int _bcm_esw_regex_policer_traverse(int unit, bcm_policer_traverse_cb cb,
                                    void *user_data)
{
    _bcm_esw_regex_device_info_t *device;          /* Regex control structure. */
    bcm_policer_config_t          cfg;             /* Policer configuration.   */
    int                           rv = BCM_E_NONE; /* Operation return status. */
    int                           idx;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Iterate over all entries. */
    for (idx = 0; idx < sizeof(device->policers)/sizeof(device->policers[0]); idx++) {
        if (NULL != device->policers[idx]) {
            sal_memcpy(&cfg, &device->policers[idx]->cfg, sizeof(cfg));
            rv = (*cb)(unit, device->policers[idx]->pid, &cfg, user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    ESW_REGEX_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      _regex_policy_policer_detach
 * Purpose:
 *      Detach a policer from a regex policy
 * Parameters:
 *      unit     - (IN) Unit number.
 *      p_ent    - (IN) Regex policy structure.
 *      level    - (IN) Policer level.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _regex_policy_policer_detach(int unit, _regex_policy_t *p_ent, int level)
{
    _field_policer_t       *f_pl;    /* Internal policer descriptor.  */
    _field_entry_policer_t *f_ent_pl;/* Field entry policer structure.*/

    /* Make sure policer attached to the entry. */
    f_ent_pl = p_ent->policer + level;
    if (0 == (f_ent_pl->flags & _FP_POLICER_VALID)) {
        return (BCM_E_EMPTY);
    }

    /* Get policer description structure. */
    BCM_IF_ERROR_RETURN(_bcm_regex_policer_get(unit, f_ent_pl->pid, &f_pl));

    /* If entry was installed decrement hw reference counter. */
    BCM_IF_ERROR_RETURN(_regex_policer_hw_free(unit, level, p_ent));

    /* Decrement policer reference counter. */
    f_pl->sw_ref_count--;

    /* If no one else is using the policer and
     * policer is internal destroy it.
     */
    if ((1 == f_pl->sw_ref_count) && (f_pl->hw_flags & _FP_POLICER_INTERNAL)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_regex_policer_destroy(unit, f_ent_pl->pid));
    }

    /* Detach policer from an entry. */
    f_ent_pl->pid   = _FP_INVALID_INDEX;
    f_ent_pl->flags = 0;

    /* Mark entry for reinstall. */
    p_ent->flags |= _FP_ENTRY_DIRTY;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_policer_attach
 * Purpose:
 *      Attach a policer to a regex policy.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      policy     - (IN) Regex policy ID.
 *      level      - (IN) Policer level.
 *      policer_id - (IN) Policer ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_policer_attach(int unit,
                                        bcm_regex_policy_t policy,
                                        int level,
                                        bcm_policer_t policer_id)
{
    _bcm_esw_regex_device_info_t *device;   /* Regex control structure.      */
    _regex_policy_t              *p_ent;    /* Allocated entry buffer.       */
    _field_entry_policer_t       *f_ent_pl; /* Field entry policer structure.*/
    _field_policer_t             *f_pl;     /* Internal policer descriptor.  */
    int                           idx;      /* Entry policers iterator.      */
    int                           rv;       /* Operation return status.      */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check. */
    if ((level >= _REGEX_POLICER_LEVEL_COUNT) || (level < 0)) {
        return (BCM_E_PARAM);
    }

    if (!BCM_POLICER_IS_REGEX_METER(policer_id)) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Get policy description structure. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* Check if another policer already attached at this level. */
    f_ent_pl = p_ent->policer + level;
    if (f_ent_pl->flags & _FP_POLICER_VALID) {
        ESW_REGEX_UNLOCK(unit);
        return (BCM_E_EXISTS);
    }

    /* Check if policer already attached to the entry. */
    for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = p_ent->policer + idx;
        if (f_ent_pl->pid == policer_id) {
            ESW_REGEX_UNLOCK(unit);
            return (idx == level) ? (BCM_E_NONE) : (BCM_E_PARAM);
        }
    }

    /* Get policer description structure. */
    rv = _bcm_regex_policer_get(unit, policer_id, &f_pl);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    /* Check that policer can be shared. */
    if (f_pl->sw_ref_count > 1) {
        /* Policer sharing is restricted to a single level. */
        if (level != f_pl->level) {
            ESW_REGEX_UNLOCK(unit);
            return (BCM_E_PARAM);
        }
    }

    /* Increment policer reference counter. */
    f_pl->sw_ref_count++;

    /* Set policer attachment level. */
    f_pl->level = level;

    /* Attach policer to an entry. */
    f_ent_pl = p_ent->policer + level;
    f_ent_pl->flags |= _FP_POLICER_VALID;
    f_ent_pl->pid    = policer_id;

    /* Entry must be reinstalled for policer to take effect. */
    p_ent->flags  |= _REGEX_POLICY_DIRTY;

    ESW_REGEX_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_regex_policy_policer_detach
 * Purpose:
 *      Detach a policer from a regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) Regex policy ID.
 *      level  - (IN) Policer level.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_policer_detach(int unit,
                                        bcm_regex_policy_t policy,
                                        int level)
{
    _bcm_esw_regex_device_info_t *device;   /* Regex control structure.      */
    _regex_policy_t              *p_ent;    /* Allocated entry buffer.       */
    int                           rv;       /* Operation return status.      */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check. */
    if ((level >= _REGEX_POLICER_LEVEL_COUNT) || (level < 0)) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Get entry description structure. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    rv = _regex_policy_policer_detach(unit, p_ent, level);

    ESW_REGEX_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_policy_policer_detach_all
 * Purpose:
 *      Detach all policers from a regex policy.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      policy - (IN) Regex policy ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_policer_detach_all(int unit,
                                            bcm_regex_policy_t policy)
{
    int       idx;                /* Entry policers iterator. */
    int       rv = BCM_E_NONE;    /* Operation return status. */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Detach all the policers attached to an entry. */
    for (idx = 0; idx < _REGEX_POLICER_LEVEL_COUNT; idx++) {
        rv =  bcm_esw_regex_policy_policer_detach(unit, policy, idx);
        if (BCM_E_EMPTY == rv) {
            /* No policer at this level. */
            rv = BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_policy_policer_get
 * Purpose:
 *      Get the policer(s) attached to a regex policy.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      policy     - (IN) Regex policy ID.
 *      level      - (IN) Policer level.
 *      policer_id - (OUT) Policer ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_policer_get(int unit,
                                     bcm_regex_policy_t policy,
                                     int level,
                                     bcm_policer_t *policer_id)
{
    _bcm_esw_regex_device_info_t *device;   /* Regex control structure.      */
    _field_entry_policer_t       *f_ent_pl; /* Field entry policer structure.*/
    _regex_policy_t              *p_ent;    /* Regex policy structure.       */
    int                           rv;       /* Operation return status.      */

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check. */
    if ((level >= _REGEX_POLICER_LEVEL_COUNT) || (level < 0)) {
        return (BCM_E_PARAM);
    }

    if (NULL == policer_id) {
        return (BCM_E_PARAM);
    }

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        return BCM_E_INIT;
    }

    if (0 == (device->flags & BCM_REGEX_USE_POLICY_ID)) {
        return BCM_E_PARAM;
    }

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    /* Get entry description structure. */
    rv = _regex_policy_get(unit, policy, &p_ent);
    if (BCM_FAILURE(rv)) {
        ESW_REGEX_UNLOCK(unit);
        return (rv);
    }

    f_ent_pl = p_ent->policer + level;
    /* Make sure policer attached to the entry. */
    if (0 == (f_ent_pl->flags & _FP_POLICER_VALID)) {
        rv = (BCM_E_NOT_FOUND);
    } else {
        *policer_id = f_ent_pl->pid;
    }

    ESW_REGEX_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_regex_stat_create
 * Purpose:
 *      Create stat collection entity.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      flags    - (IN) REGEX_STAT_xxx flags
 *      nstat    - (IN) Number of elements in stat_arr - counter types array.
 *      stat_arr - (IN) Array of counters to be collected in statistics entity.
 *      stat_id  - (OUT) Statistics entity ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_stat_create(int unit,
                              int flags,
                              int nstat,
                              bcm_field_stat_t *stat_arr,
                              int *stat_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_stat_destroy
 * Purpose:
 *      Create stat collection entity.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      stat_id - (IN) Statistics entity ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_stat_destroy(int unit,
                               int stat_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_policy_stat_attach
 * Purpose:
 *      Attach statistics entity to Regex policy.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      policy  - (IN) Regex policy ID.
 *      stat_id - (IN) Statistics entity ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_stat_attach(int unit,
                                     bcm_regex_policy_t policy,
                                     int stat_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_policy_stat_detach
 * Purpose:
 *      Detach statistics entity from Regex policy.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      policy  - (IN) Regex policy ID.
 *      stat_id - (IN) Statistics entity ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_stat_detach(int unit,
                                     bcm_regex_policy_t policy,
                                     int stat_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_policy_stat_get
 * Purpose:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      policy  - (IN) Regex policy ID.
 *      stat_id - (OUT) Statistics entity ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_policy_stat_get(int unit,
                                  bcm_regex_policy_t policy,
                                  int *stat_id)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_add
 * Purpose:
 *      Adds or updates a session entry.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      flags   - (IN) REGEX_SESSION_xxx flags.
 *      key     - (IN) The session key.
 *      session - (IN) The session data.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_add(int unit,
                              int flags,
                              bcm_regex_session_key_t *key,
                              bcm_regex_session_t *session)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_add(unit, flags, key, session);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_policy_update
 * Purpose:
 *      Updates the policy_id field in an already existing session entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      flags      - (IN) REGEX_SESSION_xxx flags.
 *      flow_index - (IN) The session index.
 *      policy     - (IN) The session policy ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_policy_update(int unit, int flags, int flow_index,
                                         bcm_regex_policy_t policy)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_policy_update(unit, flags, flow_index, policy);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_delete_all
 * Purpose:
 *      Deletes all entries from the regex session table.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_delete_all(int unit)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_delete_all(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_delete
 * Purpose:
 *      Deletes an entry from the regex session table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key  - (IN) The session key.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_delete(int unit,
                                 bcm_regex_session_key_t *key)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_delete(unit, key);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_get
 * Purpose:
 *      Returns the session data (if any).
 * Parameters:
 *      unit    - (IN) Unit number.
 *      flags   - (IN) REGEX_SESSION_xxx flags.
 *      key     - (IN) The session key.
 *      session - (OUT) The session data.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_get(int unit,
                              int flags,
                              bcm_regex_session_key_t *key,
                              bcm_regex_session_t *session)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_get(unit, flags, key, session);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_regex_session_traverse
 * Purpose:
 *      Traverse regex sessions.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      flags     - (IN) REGEX_SESSION_xxx flags.
 *      cb        - (IN) A pointer to the callback function to call for each session
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_esw_regex_session_traverse(int unit,
                                   int flags,
                                   bcm_regex_session_traverse_cb cb,
                                   void *user_data)
{
    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        return _bcm_tr3_regex_session_traverse(unit, flags, cb, user_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    return BCM_E_UNAVAIL;
}

void _bcm_esw_policy_dump(int unit, bcm_regex_policy_t policy)
{
    _regex_policy_t              *policy_ent; /* Regex policy structure.  */
    _regex_policy_action_t       *pa;
    _bcm_esw_regex_device_info_t *device;     /* Regex control structure. */
    int policer_printed = 0;
    int i;

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        LOG_CLI((BSL_META_U(unit,
                            "Regex module not initialized.\n")));
        return;
    }

    /* List the global policy data */
    LOG_CLI((BSL_META_U(unit,
                        "Number of policies: %d\n"), device->num_policies));
    LOG_CLI((BSL_META_U(unit,
                        "Minimum index: %d\n"), device->policy_index_min));
    LOG_CLI((BSL_META_U(unit,
                        "Maximum index: %d\n"), device->policy_index_max));

    if (NULL == device->policies) {
        LOG_CLI((BSL_META_U(unit,
                            "No policies are created.\n")));
        return;
    }

    ESW_REGEX_LOCK(unit);

    if (_REGEX_INVALID_INDEX == policy) {
        /* List all the policy IDs */
        for (i = 0; i < device->num_policies; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "Policy %d\n"), device->policies[i]->eid));
        }
    } else {
        /* List all the policy data */
        if (BCM_SUCCESS(_regex_policy_get(unit, policy, &policy_ent))) {
            LOG_CLI((BSL_META_U(unit,
                                "Policy %d, flags = 0x%x\n"), policy_ent->eid, policy_ent->flags));
            LOG_CLI((BSL_META_U(unit,
                                "Actions:\n")));
            pa = policy_ent->actions;
            if (NULL == pa) {
                LOG_CLI((BSL_META_U(unit,
                                    "None.\n")));
            } else {
                do {
                    LOG_CLI((BSL_META_U(unit,
                                        "\t%s: %d, %d\n"), _regex_policy_action_name(pa->action), pa->param[0], pa->param[1]));
                    pa = pa->next;
                } while (NULL != pa);
                if (policy_ent->statistic.sid != _REGEX_INVALID_INDEX) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Statistic:\n")));
                    LOG_CLI((BSL_META_U(unit,
                                        "\tID %d (0x%x)\n"), policy_ent->statistic.sid, policy_ent->statistic.sid));
                }
                for (i = 0; i < _REGEX_POLICER_LEVEL_COUNT; i++) {
                    if (policy_ent->policer[i].pid != _FP_INVALID_INDEX) {
                        if (!policer_printed) {
                            LOG_CLI((BSL_META_U(unit,
                                                "Policers:\n")));
                            policer_printed = 1;
                        }
                        LOG_CLI((BSL_META_U(unit,
                                            "\tID %d (0x%x)\n"), policy_ent->policer[i].pid, policy_ent->policer[i].pid));
                    }
                }
            }
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Policy not found.\n")));
        }
    }

    ESW_REGEX_UNLOCK(unit);
}

void _bcm_esw_policer_dump(int unit, bcm_policer_t policer_id)
{
    _bcm_esw_regex_device_info_t *device; /* Regex control structure. */
    _field_policer_t             *f_pl;   /* Internal policer descriptor.  */
    int                           i;
    int                           count = 0;

    if (!soc_feature(unit, soc_feature_regex)) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Get the device info structure. */
    device = ESW_REGEX_INFO(unit);
    if (NULL == device) {
        LOG_CLI((BSL_META_U(unit,
                            "Regex module not initialized.\n")));
        return;
    }

    /* List the global policy data */
    LOG_CLI((BSL_META_U(unit,
                        "Maximum number of policers: %zu\n"), sizeof(device->policers)/sizeof(device->policers[0])));

    /* Lock the module. */
    ESW_REGEX_LOCK(unit);

    if (_REGEX_INVALID_INDEX == policer_id) {
        /* Iterate over all entries. */
        for (i = 0; i < sizeof(device->policers)/sizeof(device->policers[0]); i++) {
            if (NULL != device->policers[i]) {
                LOG_CLI((BSL_META_U(unit,
                                    "Policer %d (0x%x)\n"), device->policers[i]->pid, device->policers[i]->pid));
                count++;
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "Policers in use: %d\n"), count));
    } else {
        i = _REGEX_POLICER_ID_TO_INDEX(policer_id);
        if ((NULL != device->policers[i]) &&
            (device->policers[i]->pid == policer_id) &&
            (BCM_SUCCESS(_bcm_regex_policer_get(unit, device->policers[i]->pid, &f_pl)))) {
            LOG_CLI((BSL_META_U(unit,
                                "Policer %d (0x%x), flags = 0x%x\n"), f_pl->pid, f_pl->pid, f_pl->hw_flags));
            LOG_CLI((BSL_META_U(unit,
                                "\tSW Ref Count = %d, HW Ref Count = %d\n"), f_pl->sw_ref_count, f_pl->hw_ref_count));
            LOG_CLI((BSL_META_U(unit,
                                "\tLevel = %d, Pool Index = %d, HW Index = %d\n"), f_pl->level, f_pl->pool_index, f_pl->hw_index));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Policer not found.\n")));
        }
    }

    ESW_REGEX_UNLOCK(unit);
}
#endif /* INCLUDE_REGEX */
